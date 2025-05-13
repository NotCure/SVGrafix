#include <iostream>
#include <vector>


#include "SVGElement.h"
#include "error/ParseError.h"



namespace css {
	struct Declaration // "fill: #000000;"
	{
		std::string property; // "fill"
		std::string value; // "#000000"
		int specificity; // "(id, class, tag)"
	};

	struct Rule // "div { fill: #000000; }"
	{
		std::vector<std::string> selectors; // "div", ".class", "#id"
		std::vector<Declaration> declarations; // "fill: #000000;"
	};

    static bool matches(const SVGElement& el, std::string_view sel) {
        if (sel.empty()) return false;
        if (sel[0] == '.') {           
            auto cls = el.get_attr("class");
            size_t p = 0;
            while (p < cls.size()) {
                auto nxt = cls.find(' ', p);
                auto part = cls.substr(p, nxt == std::string::npos ? cls.size() : nxt - p);
                if (part == sel.substr(1)) return true;
                if (nxt == std::string::npos) break;
                p = nxt + 1;
            }
            return false;
        }
        else if (sel[0] == '#')  {
            return el.get_attr("id") == sel.substr(1);
        }
        else{
            return std::equal(sel.begin(), sel.end(),
                el.tag().begin(), el.tag().end(),
                [](char a, char b) { return tolower(a) == tolower(b); });
        }
    }




	class StyleSheet
	{
	public:
		void add_rule(Rule rule) {
			rules_.push_back(std::move(rule));
		}




		std::unordered_map<std::string, std::string> 
            declartions_for(const SVGElement& el) const {
            std::unordered_map<std::string, std::string> out;
            std::unordered_map<std::string, int>          best_spec;

            for (auto const& rule : rules_) {
                for (auto const& sel : rule.selectors) {
                    if (!matches(el, sel)) continue;
                    for (auto const& d : rule.declarations) {
                        auto it = best_spec.find(d.property);
                        if (it == best_spec.end() || d.specificity >= it->second) {
                            out[d.property] = d.value;
                            best_spec[d.property] = d.specificity;
                        }
                    }
                }
            }
            return out;

		} 

	private:
		std::vector<Rule> rules_;

	};



    inline void parse_into(StyleSheet& sheet,
        std::string_view cssText,
        error::ParseResult& result)
    {
        size_t pos = 0, n = cssText.size();
        while (pos < n) {
            pos = cssText.find_first_not_of(" \t\r\n", pos);
            if (pos == std::string_view::npos) break;

            auto brace = cssText.find('{', pos);
            if (brace == std::string_view::npos) {
                result.errors.emplace_back(
                    error::ErrorCode::CSS_SyntaxError, pos,
                    "Expected '{' to open CSS rule");
                break;    
            }

            std::string_view sels = cssText.substr(pos, brace - pos);
            {
                auto a = sels.find_first_not_of(" \t\r\n");
                auto b = sels.find_last_not_of(" \t\r\n");
                if (a == std::string_view::npos) {
                    result.errors.emplace_back(
                        error::ErrorCode::CSS_BadSelector, pos,
                        "Empty selector before '{'");
                    pos = brace + 1;
                    continue;
                }
                sels = sels.substr(a, b - a + 1);
            }

            auto endb = cssText.find('}', brace + 1);
            if (endb == std::string_view::npos) {
                result.errors.emplace_back(
                    error::ErrorCode::CSS_UnexpectedEOF, brace,
                    "Expected '}' to close CSS rule");
                break;
            }

            auto nextOpen = cssText.find('{', brace + 1);
            if (nextOpen != std::string_view::npos && nextOpen < endb) {
                result.errors.emplace_back(
                    error::ErrorCode::CSS_SyntaxError, nextOpen,
                    "Unexpected '{' before closing '}'");
                // skip past this bad rule
                pos = endb + 1;
                continue;
            }

            std::string_view decls =
                cssText.substr(brace + 1, endb - (brace + 1));

            pos = endb + 1;

            Rule rule;
            size_t sp = 0;
            while (sp < sels.size()) {
                auto comma = sels.find(',', sp);
                auto part = sels.substr(
                    sp,
                    (comma == std::string_view::npos
                        ? sels.size()
                        : comma)
                    - sp);
                auto a = part.find_first_not_of(" \t\r\n");
                auto b = part.find_last_not_of(" \t\r\n");
                if (a == std::string_view::npos) {
                    result.errors.emplace_back(
                        error::ErrorCode::CSS_BadSelector, sp,
                        "Empty selector in comma-list");
                }
                else {
                    rule.selectors.push_back(
                        std::string(part.substr(a, b - a + 1)));
                }
                if (comma == std::string_view::npos) break;
                sp = comma + 1;
            }

            size_t dp = 0;
            while (dp < decls.size()) {
                auto semi = decls.find(';', dp);
                auto piece = decls.substr(
                    dp,
                    (semi == std::string_view::npos
                        ? decls.size()
                        : semi)
                    - dp);
                dp = (semi == std::string_view::npos)
                    ? decls.size()
                    : (semi + 1);

                auto colon = piece.find(':');
                if (colon == std::string_view::npos) {
                    result.errors.emplace_back(
                        error::ErrorCode::CSS_BadDeclaration, dp,
                        "Missing ':' in declaration '" + std::string(piece) + "'");
                    continue;
                }

                std::string prop(piece.substr(0, colon));
                std::string val(piece.substr(colon + 1));
                auto a1 = prop.find_first_not_of(" \t\r\n");
                auto b1 = prop.find_last_not_of(" \t\r\n");
                auto a2 = val.find_first_not_of(" \t\r\n");
                auto b2 = val.find_last_not_of(" \t\r\n");
                if (a1 == std::string::npos || a2 == std::string::npos) {
                    result.errors.emplace_back(
                        error::ErrorCode::CSS_BadDeclaration, dp,
                        "Empty name or value in '" + std::string(piece) + "'");
                    continue;
                }
                prop = prop.substr(a1, b1 - a1 + 1);
                val = val.substr(a2, b2 - a2 + 1);

                int spec = 1;
                if (!rule.selectors.empty()) {
                    auto& sel = rule.selectors.back();
                    if (sel[0] == '#') spec = 100;
                    else if (sel[0] == '.') spec = 10;
                }
                rule.declarations.push_back({ prop, val, spec });
            }

            if (!rule.selectors.empty() && !rule.declarations.empty()) {
                sheet.add_rule(std::move(rule));
            }
        }
    }


}