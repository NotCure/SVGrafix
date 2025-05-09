#include <utility/String.h>
#include <algorithm>
#include <cctype>

namespace utility::str {

    std::string to_lower(std::string_view input)
    {
        std::string out(input);
        std::transform(out.begin(), out.end(), out.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return out;
    }

}