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

inline const std::unordered_map<std::string_view, DomSVG::Factory> DomSVG::registry_ = {
    { "svg",   [](std::string_view body) {
        auto e = std::make_unique<SVG>();
        e->parseAttributes(body);
        return e;
    }},
    { "rect",  [](std::string_view body) {
        auto e = std::make_unique<SVGRect>();
        e->parseAttributes(body);
        return e;
    }},
};

DomSVG::DomSVG(std::string content)
    : text_(std::move(content))
{

    remove_prologue();
    remove_comments();
    auto start = std::chrono::high_resolution_clock::now();
    parse();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;

}

error::ParseResult DomSVG::parse()
{
    error::ParseResult result;
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
   

        if (end == std::string::npos) break;

        /* ---- extract tag-name ------------------------------------------------ */
        std::size_t nstart = pos + 1;
        std::size_t nend = view.find_first_of(" \t/>", nstart);
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
        if (it != registry_.end())
        {
            auto elem = it->second(attrBody);
            elements_.emplace_back(std::move(elem));
        }

        pos = end + 1;
    }
    return result;
}
void DomSVG::remove_prologue() {
    std::vector<std::pair<size_t, size_t>> ranges;
    for (size_t pos = 0; (pos = text_.find("<?", pos)) != std::string::npos; ) {
        size_t end = text_.find("?>", pos + 2);
        if (end == std::string::npos) break;              
        ranges.emplace_back(pos, end + 1);                
        pos = end + 2;
    }
    copy_excluding_ranges(text_, ranges, no_prologue_);

}


void DomSVG::remove_comments() {
    std::vector<std::pair<size_t, size_t>> ranges;
    for (size_t pos = 0; (pos = no_prologue_.find("<!--", pos)) != std::string::npos; ) {
        size_t end = no_prologue_.find("-->", pos + 4);
        if (end == std::string::npos) break;              
        ranges.emplace_back(pos, end + 2);
        pos = end + 3;
    }
    copy_excluding_ranges(no_prologue_, ranges, no_comments_);



}


const DomSVG::ElementList& DomSVG::getElements() const noexcept {
    return elements_;
}

void DomSVG::debugPrint(std::ostream& os) const
{
    for (const auto& e : elements_) {
   
		os << e->tag() << ":\n";
		for (const auto& [key, value] : e->getAttributes()) {
			os << "  " << key << ": " << value << "\n";
		}
		os << "\n";
    }
}