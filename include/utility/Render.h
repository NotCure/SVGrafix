#pragma once
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



    inline float to_float(std::string_view sv)
    {
        char* end = nullptr;
        std::string c(sv);
        return std::strtof(c.c_str(), &end);
    }

}