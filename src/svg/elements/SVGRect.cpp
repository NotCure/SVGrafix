#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <svg/elements/Factory.h>
#include <svg/elements/SVGRect.h>
#include <utility/Render.h>

REGISTER_SVG_ELEMENT("rect", SVGRect)



RectGeom SVGRect::geometry() const
{
    return { utility::render::to_float(get_attr("x")),
             utility::render::to_float(get_attr("y")),
             utility::render::to_float(get_attr("width")),
             utility::render::to_float(get_attr("height")) };
}

TessOutput SVGRect::tessellate() const
{
    auto g = geometry();         
    return {
        { g.x, g.y, 0,
          g.x + g.w, g.y, 0,
          g.x + g.w, g.y + g.h, 0,
          g.x, g.y + g.h, 0 },
        { 0,1,2,  0,2,3 }
    };
}