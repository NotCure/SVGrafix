#include <utility/Color.h>

#include <string>
#include <algorithm>

static float hex2f(char c)
{
    if ('0' <= c && c <= '9') return (c - '0') / 15.f;
    if ('a' <= c && c <= 'f') return (c - 'a' + 10) / 15.f;
    if ('A' <= c && c <= 'F') return (c - 'A' + 10) / 15.f;
    return 0.f;
}

Rgba parse_color(std::string_view css)
{
    Rgba c{ 1,1,1,1 };      
    if (css.empty()) return c;

    if (css[0] == '#' && (css.size() == 7 || css.size() == 9))
    {
        c.r = hex2f(css[1]) * 16.f + hex2f(css[2]);
        c.g = hex2f(css[3]) * 16.f + hex2f(css[4]);
        c.b = hex2f(css[5]) * 16.f + hex2f(css[6]);
        if (css.size() == 9)
            c.a = hex2f(css[7]) * 16.f + hex2f(css[8]);
    }
    else if (css == "red")            
        c = { 1,0,0,1 };
    else if (css == "green")          
        c = { 0,1,0,1 };
    else if (css == "blue")           
        c = { 0,0,1,1 };
    else if (css == "black")          
        c = { 0,0,0,1 };
    else if (css == "white")          
        c = { 1,1,1,1 };
  
    return c;
}
