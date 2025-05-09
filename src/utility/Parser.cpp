#include <utility/Parser.h>
#include <cctype>

namespace utility::parser {

    void parse_attributes(std::string_view tag_body,
        std::unordered_map<std::string, std::string>& attrs)
    {
        std::size_t i = 0, n = tag_body.size();

        while (i < n) {
            while (i < n && std::isspace(static_cast<unsigned char>(tag_body[i]))) ++i;
            if (i >= n) break;

            // --- name ---------------------------------------------------------
            std::size_t name_start = i;
            while (i < n && (std::isalnum(static_cast<unsigned char>(tag_body[i])) ||
                tag_body[i] == '-' || tag_body[i] == ':'))
                ++i;
            if (i == name_start) break;
            std::string name(tag_body.substr(name_start, i - name_start));

            while (i < n && std::isspace(static_cast<unsigned char>(tag_body[i]))) ++i;
            if (i >= n || tag_body[i] != '=') break;
            ++i;

            while (i < n && std::isspace(static_cast<unsigned char>(tag_body[i]))) ++i;
            if (i >= n || (tag_body[i] != '"' && tag_body[i] != '\'')) break;
            char quote = tag_body[i++];

            // --- value --------------------------------------------------------
            std::size_t value_start = i;
            while (i < n && tag_body[i] != quote) ++i;
            if (i >= n) break;
            std::string value(tag_body.substr(value_start, i - value_start));
            ++i;

            attrs.emplace(std::move(name), std::move(value));
        }
    }

} 
