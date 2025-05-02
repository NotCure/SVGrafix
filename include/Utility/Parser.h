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

} 