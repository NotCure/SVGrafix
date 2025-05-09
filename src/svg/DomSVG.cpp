#include <svg/DomSVG.h>
#include <svg/error/ParseError.h>

#include <iostream>
#include <chrono>
#include <utility/String.h>

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
    { "g", &make_element<SVGGroup>},
};

DomSVG::DomSVG(std::string content)
    : text_(std::move(content))
{
    parse();


}

static bool iequals(std::string_view a, std::string_view b) noexcept
{
    using uchar = unsigned char;
    return a.size() == b.size() &&
        std::ranges::equal(a, b, [](char x, char y) {
        return std::tolower(static_cast<uchar>(x)) ==
            std::tolower(static_cast<uchar>(y));
            });
}

error::ParseResult DomSVG::parse()
{
    if (parsing_) throw std::logic_error("The parsing called re-entrantly");

    parsing_ = true;
    clear();

    error::ParseResult result;
    remove_prologue(result);
    remove_comments(result);

    std::string_view view(no_comments_);
	root_ = std::make_unique<SVGNode>(std::make_unique<SVG>());
    stack_.clear();
	stack_.push_back(root_.get());


    size_t pos = 0;
    while ((pos = view.find('<', pos)) != std::string::npos) {
        bool isClosing = (pos + 1 < view.size() && view[pos + 1] == '/');


        size_t end = view.find('>', pos + 1);
        if (end == std::string::npos) {
            result.errors.emplace_back(error::ErrorCode::UnexpectedEOF, pos,
                "Tag opened with '<' but never closed with '>'.");
            break;
        }

        size_t nstart = isClosing ? pos + 2 : pos + 1;
        size_t nend = view.find_first_of(" \t/>", nstart);
        if (nend == nstart) { 
            pos = end + 1; continue; 
        }
        if (nend == std::string::npos || nend > end) nend = end;

        std::string_view tagName = view.substr(nstart, nend - nstart);
        bool selfClosing = !isClosing && end > pos + 1 && view[end - 1] == '/';
        size_t astart = nend;
        size_t alen = (selfClosing ? end - 1 : end) - astart;
        std::string_view attrBody = view.substr(astart, alen);

        pos = end + 1;

        if (isClosing) {
			if (stack_.size() == 1) {
                result.errors.emplace_back(error::ErrorCode::MalformedTag,
                    nstart, "Unexpected closing </" + std::string(tagName) + ">");
				continue;
			}

	
			std::string_view open = stack_.back()->element->tag();
            if (!iequals(open, tagName)) {
                result.errors.emplace_back(error::ErrorCode::MalformedTag,
                    nstart,
                    "Closing </" + std::string(tagName) +
                    "> does not match <" + std::string(open) + ">");
            

                while (stack_.size() > 1 && !iequals(stack_.back()->element->tag(), tagName))
                    stack_.pop_back();

                if (stack_.size() > 1) stack_.pop_back();
               
            } else {
                stack_.pop_back();
            }

            continue;
        }


        std::string lowerBuf;     
        std::string_view lowerView;   

        bool has_upper = std::any_of(tagName.begin(), tagName.end(),
            [](unsigned char c) { return std::isupper(c); });

        if (has_upper) {
            lowerBuf = utility::str::to_lower(tagName);
            lowerView = lowerBuf;
        }
        else {
            lowerView = tagName;
        }


        const auto it = registry_.find(lowerView);
        if (it == registry_.end()) {
            result.errors.emplace_back(error::ErrorCode::MalformedTag, nstart,
                "Unknown tag: " + std::string(tagName));

            continue;
        }
        else {
            auto elem = it->second(attrBody);           
            auto node = std::make_unique<SVGNode>(std::move(elem));
            SVGNode* raw = node.get();

            stack_.back()->children.emplace_back(std::move(node));
            if (!selfClosing) stack_.push_back(raw);

        }


    }

    parsing_ = false;
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

void DomSVG::debug_print(std::ostream& os) const
{
    if (!root_) { os << "(empty)\n"; return; }

    std::function<void(const SVGNode*, int)> visit;
    visit = [&](const SVGNode* node, int depth)
    {
        if (!node) return;
        os << std::string(depth * 2, ' ') << "<" << node->element->tag() << ">\n";
        for (auto& [k, v] : node->element->getAttributes())
            os << std::string(depth * 2 + 2, ' ') << k << " = " << v << "\n";

        for (auto const& child : node->children)
            visit(child.get(), depth + 1);
    };
    visit(root_.get(), 0);
}


void DomSVG::clear()
{
    root_.reset();
    stack_.clear();
    no_prologue_.clear();
    no_comments_.clear();
}
