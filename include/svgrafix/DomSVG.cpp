#include "DomSVG.h"
#include "TagTypes.h"
#include "Error/ParseError.h"

#include <iostream>
#include <chrono>


static void copy_excluding_ranges(std::string_view src,
    const std::vector<std::pair<size_t, size_t>>& excludes,
    std::string& dst)
{
    dst.clear();
    dst.reserve(src.size());

    size_t cur = 0;
    for (auto [beg, end] : excludes) {
        if (cur < beg)      dst.append(src.substr(cur, beg - cur));
        cur = end + 1;      
    }
    if (cur < src.size())   dst.append(src.substr(cur));
}


template<typename T>
static std::unique_ptr<SVGElement> make_element(std::string_view body) {
    auto e = std::make_unique<T>();
    e->parseAttributes(body);
    return e;
}


inline const std::unordered_map<std::string_view, DomSVG::Factory> DomSVG::registry_ = {
    { "svg",  &make_element<SVG>    },
    { "rect", &make_element<SVGRect>},
};

DomSVG::DomSVG(std::string content)
    : text_(std::move(content))
{
    auto start = std::chrono::high_resolution_clock::now();
    parse();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;

}

error::ParseResult DomSVG::parse()
{
    error::ParseResult result;
    remove_prologue(result);
    remove_comments(result);


    std::string_view view(no_comments_);


    std::size_t openings = std::count(view.begin(), view.end(), '<');
    elements_.reserve(openings / 2);         

    std::size_t pos = 0;
    while ((pos = view.find('<', pos)) != std::string::npos)
    {
        /* skip closing tags */
        if (pos + 1 < view.size() && view[pos + 1] == '/')
        {
            pos++; continue;
        }
        
        std::size_t end = view.find('>', pos + 1);
        if (end == std::string::npos) {
            result.errors.emplace_back(
                error::ErrorCode::UnexpectedEOF,
                pos,
                "Tag opened with '<' but never closed with '>'."
            );
            break;
        }

        /* ---- extract tag-name ------------------------------------------------ */
        std::size_t nstart = pos + 1;
        std::size_t nend = view.find_first_of(" \t/>", nstart);
        if (nend == nstart) {
            result.errors.emplace_back(
                error::ErrorCode::MalformedTag,
                pos,
                "Empty tag name between '<' and '>'."
            );
            pos = end + 1;
            continue;
        }
        if (nend == std::string::npos || nend > end) nend = end;
        std::string_view tagName = view.substr(nstart, nend - nstart);

        /* ---- attribute substring (exclude trailing '/' if self-closing) ------ */
        bool selfClosing = end > pos + 1 && view[end - 1] == '/';
        std::size_t astart = nend;
        std::size_t alen = (selfClosing ? end - 1 : end) - astart;
        std::string_view attrBody = view.substr(astart, alen);

        /* ---- cheap, allocation-free lowercase key --------------------------- */
        std::string_view lookupKey;
        bool needsLower = false;
        for (char c : tagName)
            if (static_cast<unsigned char>(c) >= 'A' && c <= 'Z')
            {
                needsLower = true; break;
            }

        if (!needsLower)    
        {
            lookupKey = tagName;                  
        }
        else
        {
            char tmp[30];                         
            std::size_t len = tagName.size();
            if (len >= sizeof(tmp)) { pos = end + 1; continue; } 
            for (std::size_t i = 0; i < len; ++i)
                tmp[i] = static_cast<char>(
                    std::tolower(static_cast<unsigned char>(tagName[i])));
            lookupKey = std::string_view(tmp, len); 
        }

        /* ---- create element if recognised ----------------------------------- */
        auto it = registry_.find(lookupKey);
        if (it == registry_.end()) {
            result.errors.emplace_back(
                error::ErrorCode::MalformedTag,
                nstart,
                std::string("Unknown tag: ") + std::string(tagName)
            );
        }
        else {
            elements_.emplace_back(it->second(attrBody));
        }

        pos = end + 1;
    }
    return result;
}

void DomSVG::remove_prologue(error::ParseResult& result) {
    std::vector<std::pair<size_t, size_t>> ranges;
    size_t pos = 0;

    while (true) {
        size_t beg = text_.find("<?", pos);
        if (beg == std::string::npos)
            break;

        size_t close = text_.find("?>", beg + 2);
        size_t nested = text_.find("<?", beg + 2);

        if (close == std::string::npos ||
            (nested != std::string::npos && nested < close))
        {
            result.errors.emplace_back(
                error::ErrorCode::UnexpectedEOF,
                beg,
                "Found '<?' without matching '?>'"
            );
            pos = (nested != std::string::npos) ? nested : beg + 2;
            continue;
        }

        ranges.emplace_back(beg, close + 1);  
        pos = close + 2;
    }

    copy_excluding_ranges(text_, ranges, no_prologue_);
}


void DomSVG::remove_comments(error::ParseResult& result) {
    std::vector<std::pair<size_t, size_t>> ranges;
    size_t pos = 0;

    while (true) {
        size_t beg = no_prologue_.find("<!--", pos);
        if (beg == std::string::npos) break;

        size_t close = no_prologue_.find("-->", beg + 4);
        size_t nested = no_prologue_.find("<!--", beg + 4);

        if (close == std::string::npos || (nested != std::string::npos && nested < close)) {
            result.errors.emplace_back(
                error::ErrorCode::UnclosedComment,
                beg,
                "Found '<!--' without matching '-->' before next '<!--'"
            );
            pos = (nested != std::string::npos) ? nested : beg + 4;
            continue;
        }
        ranges.emplace_back(beg, close + 2);  
        pos = close + 3;
    }

    copy_excluding_ranges(no_prologue_, ranges, no_comments_);
}


const DomSVG::ElementList& DomSVG::get_elements() const noexcept {
    return elements_;
}

void DomSVG::debug_print(std::ostream& os) const
{
    for (const auto& e : elements_) {
   
		os << e->tag() << ":\n";
		for (const auto& [key, value] : e->getAttributes()) {
			os << "  " << key << ": " << value << "\n";
		}
		os << "\n";
    }
}