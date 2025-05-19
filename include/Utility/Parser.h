// ---------- Parser.h ----------
#pragma once
#include <string_view>
#include <string>
#include <unordered_map>

namespace utility::parser {

    void parse_attributes(
        std::string_view tag_body,
        std::unordered_map<std::string, std::string>& out_attributes
    );

    static inline std::string_view trim(std::string_view v) noexcept
    {
        const auto first = v.find_first_not_of(" \t\r\n");
        if (first == std::string_view::npos) return {};
        const auto last = v.find_last_not_of(" \t\r\n");
        return v.substr(first, last - first + 1);
    }

} 