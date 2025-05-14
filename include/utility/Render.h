#include <iostream>
#include<glad/glad.h>







struct Vec2 { float x, y; };
struct Vec4 { float r, g, b, a; };

struct RenderItem
{
    GLuint vao = 0;
    GLsizei indexCount = 0;
    Vec4 fill = { 1,1,1,1 };
};

namespace utility::render {

	inline Vec4 parse_hex_color(std::string_view hex) {
        auto hexByte = [](char hi, char lo) {
            auto toNibble = [](char c)->int {
                return (c <= '9') ? c - '0'
                    : (c <= 'F') ? c - 'A' + 10
                    : c - 'a' + 10;
            };
            return (toNibble(hi) << 4) | toNibble(lo);
        };
        if (hex.size() == 7 && hex[0] == '#')
        {
            int r = hexByte(hex[1], hex[2]);
            int g = hexByte(hex[3], hex[4]);
            int b = hexByte(hex[5], hex[6]);
            return { r / 255.f, g / 255.f, b / 255.f, 1.0f };
        }
        return { 0,0,0,1 };
	}

    inline float to_float(std::string_view sv)
    {
        char* end = nullptr;
        std::string c(sv);
        return std::strtof(c.c_str(), &end);
    }

}