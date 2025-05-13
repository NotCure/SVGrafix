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
    : text_(std::move(content)){}

static bool iequals(std::string_view a, std::string_view b) noexcept
{
    using uchar = unsigned char;
    return a.size() == b.size() &&
        std::ranges::equal(a, b, [](char x, char y) {
        return std::tolower(static_cast<uchar>(x)) ==
            std::tolower(static_cast<uchar>(y));
            });
}

enum class State {
    Text,
	TagOpen,
    Comment,
    Prologue,
    Style,
    Element
};
static bool iequals_n(const char* a, const char* b, size_t n) {
    for (size_t i = 0; i < n; ++i)
        if (std::tolower((unsigned char)a[i]) != std::tolower((unsigned char)b[i]))
            return false;
    return true;
}
error::ParseResult DomSVG::parse() {

	if (parsing_) throw std::logic_error("The parsing called re-entrantly");
    parsing_ = true;
    clear();

	error::ParseResult result;
   

    css::StyleSheet& sheet = sheet_;
	const std::string& src = text_;
	size_t n = src.size();
    size_t i = 0;
	State st = State::Text;


    auto match = [&](size_t j, const char* s) {
        auto len = std::strlen(s);
        return j + len <= n && std::memcmp(src.data() + j, s, len) == 0;
    };


	root_ = std::make_unique<SVGNode>(std::make_unique<SVG>());
	stack_.push_back(root_.get());


    while (i < n) {
        switch (st) {
        case State::Text: {
            if (src[i] == '<') {
                if (match(i, "<!--")) {
                    st = State::Comment;  i += 4;
                }
                else if (match(i, "<?")) {
                    st = State::Prologue; i += 2;
                }
                else if (iequals_n(src.data() + i, "<style", 6)) {
                    st = State::Style;    i += 6;
                    while (i < n && src[i] != '>') ++i;
                    if (i < n) ++i;
                }
                else {
                    st = State::TagOpen;  ++i;
                }
            }
            else {
                ++i;
            }
            break;
        }
        case State::Comment: {
            auto pos2 = src.find("-->", i);
            if (pos2 == std::string::npos) {
                result.errors.emplace_back(
                    error::ErrorCode::UnclosedComment, i,
                    "Found '<!--' without matching '-->'");
                i = n;
            }
            else {
                i = pos2 + 3;
                st = State::Text;
            }
            break;
        }


        case State::Prologue: {
            auto pos2 = src.find("?>", i);
            if (pos2 == std::string::npos) {
                result.errors.emplace_back(
                    error::ErrorCode::UnexpectedEOF, i,
                    "Found '<?' without matching '?>'");
                i = n;
            }
            else {
                i = pos2 + 2;
                st = State::Text;
            }
            break;
        }
        case State::Style: {
            auto pos2 = src.find("</style>", i);
            std::string cssText;
            if (pos2 == std::string::npos) {
                result.errors.emplace_back(
                    error::ErrorCode::UnexpectedEOF, i,
                    "Found '<style>' without matching '</style>'");
                cssText.assign(src, i, n - i);
                i = n;
            }
            else {
                cssText.assign(src, i, pos2 - i);
                i = pos2 + 8;
            }
			std::cout << "CSS: " << cssText << "\n";
            css::parse_into(sheet, cssText, result);
            st = State::Text;
            break;
        }
        case State::TagOpen: {
            // Are we closing?
            bool isClosing = (i < n && src[i] == '/');
            if (isClosing) ++i;

            // 1) parse tag name
            size_t nameStart = i;
            while (i < n) {
                char c = src[i];
                if (std::isspace((unsigned char)c) || c == '>' || c == '/') break;
                ++i;
            }
            std::string_view tagName{ src.data() + nameStart, i - nameStart };

            // 2) parse attributes until '>'
            size_t attrStart = i;
            bool selfClosing = false;
            while (i < n && src[i] != '>') {
                if (src[i] == '/' && (i + 1 < n && src[i + 1] == '>')) {
                    selfClosing = true;
                    i += 2;  // skip "/>"
                    break;
                }
                ++i;
            }
            size_t attrEnd = i;
            if (i < n && src[i] == '>') ++i;  // skip '>'
            std::string_view attrBody{ src.data() + attrStart, attrEnd - attrStart };

            // 3) dispatch
            if (isClosing) {
                // closing tag: must match top of stack
                if (stack_.size() <= 1) {
                    result.errors.emplace_back(
                        error::ErrorCode::MalformedTag, nameStart,
                        "Unexpected closing </" + std::string(tagName) + ">");
                }
                else {
                    auto openTag = stack_.back()->element->tag();
                    if (!iequals_n(openTag.data(), tagName.data(), openTag.size())) {
                        result.errors.emplace_back(
                            error::ErrorCode::MalformedTag, nameStart,
                            "Closing </" + std::string(tagName) +
                            "> does not match <" + std::string(openTag) + ">");
                        // pop until we find a match (error recovery)
                        while (stack_.size() > 1 &&
                            !iequals_n(stack_.back()->element->tag().data(),
                                tagName.data(),
                                tagName.size())) {
                            stack_.pop_back();
                        }
                        if (stack_.size() > 1) stack_.pop_back();
                    }
                    else {
                        // normal pop
                        stack_.pop_back();
                    }
                }
            }
            else {
                // opening or self-closing tag
                auto lower = [](std::string_view sv) {
                    std::string out(sv);
                    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return std::tolower(c); });
                    return out;
                };
                std::string key = lower(tagName);
                auto it = registry_.find(key);
                if (it == registry_.end()) {
                    result.errors.emplace_back(
                        error::ErrorCode::MalformedTag, nameStart,
                        "Unknown tag: " + std::string(tagName));
                }
                else {
                    auto elem = it->second(attrBody);
                    auto node = std::make_unique<SVGNode>(std::move(elem));
                    SVGNode* raw = node.get();
                    stack_.back()->children.emplace_back(std::move(node));
                    if (!selfClosing) stack_.push_back(raw);
                }
            }

            st = State::Text;
            break;
        }
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
    if (!root_) {
        os << "(empty)\n";
        return;
    }

    auto indent = [&](int depth) {
        return std::string(depth * 2, ' ');
    };

    std::function<void(const SVGNode*, int)> visit;
    visit = [&](const SVGNode* node, int depth)
    {
        if (!node) return;

        os << indent(depth) << "<" << node->element->tag() << ">\n";

        for (auto& [k, v] : node->element->getAttributes()) {
            os << indent(depth + 1)
                << k << " = " << v << "\n";
        }

        {
            auto cssMap = sheet_.declartions_for(*node->element);
            if (!cssMap.empty()) {
                os << indent(depth + 1) << "[CSS styles]\n";
                for (auto& [prop, val] : cssMap) {
                    os << indent(depth + 2)
                        << prop << " : " << val << "\n";
                }
            }
        }

        for (auto const& child : node->children) {
            visit(child.get(), depth + 1);
        }
    };

    visit(root_.get(), 0);
}

void DomSVG::extract_css()
{

}

void DomSVG::clear()
{
    root_.reset();
    stack_.clear();
    no_prologue_.clear();
    no_comments_.clear();
}
