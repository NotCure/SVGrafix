#pragma once
#include <iostream>
#include <vector>




#include <svg/SVGElement.h>
#include <svg/error/ParseError.h>
#include <optional>
#include <utility/Color.h>
#include <utility/Mat3.h>



namespace css {
	struct Declaration
	{
		std::string property; 
		std::string value; 
		int specificity;
	};

    struct ComputedStyle {
        std::optional<Rgba>    fill;
        std::optional<Rgba>    stroke;
        std::optional<float>   stroke_width;
        std::optional<Mat3>    transform;
    };
	struct Rule 
	{
		std::vector<std::string> selectors; 
		std::vector<Declaration> declarations;
	};

	static bool matches(const SVGElement& el, std::string_view sel);




	class StyleSheet
	{
	public:
        void add_rule(Rule rule);

        std::unordered_map<std::string, std::string>
            declartions_for(const SVGElement& el) const;
	private:
		std::vector<Rule> rules_;

	};



    inline void parse_into(StyleSheet& sheet,
        std::string_view cssText,
        error::ParseResult& result) {
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

            auto trim = [](std::string_view sv) {
                auto a = sv.find_first_not_of(" \t\r\n");
                if (a == std::string_view::npos) return std::string_view{};
                auto b = sv.find_last_not_of(" \t\r\n");
                return sv.substr(a, b - a + 1);
            };


            size_t dp = 0;
            while (dp < decls.size()) {
                auto semi = decls.find(';', dp);
                auto piece = decls.substr(dp,
                    (semi == std::string_view::npos ? decls.size() : semi) - dp);

                dp = (semi == std::string_view::npos) ? decls.size() : semi + 1;

                piece = trim(piece);
                if (piece.empty())
                    continue;

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