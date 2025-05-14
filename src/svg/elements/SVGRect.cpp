#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <svg/elements/Factory.h>
#include <svg/elements/SVGRect.h>
#include <utility/Render.h>

REGISTER_SVG_ELEMENT("rect", SVGRect)


static Vec2 svg_to_ndc(float x, float y, float viewW, float viewH)
{
    float nx = (x / viewW) * 2.0f - 1.0f;
    float ny = -(y / viewH) * 2.0f + 1.0f;   
    return { nx, ny };
}

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
    const float viewW = 800, viewH = 800; 

    Vec2 p0 = svg_to_ndc(g.x, g.y, viewW, viewH);
    Vec2 p1 = svg_to_ndc(g.x + g.w, g.y, viewW, viewH);
    Vec2 p2 = svg_to_ndc(g.x + g.w, g.y + g.h, viewW, viewH);
    Vec2 p3 = svg_to_ndc(g.x, g.y + g.h, viewW, viewH);

    TessOutput out;
    out.verts = {
        p0.x, p0.y, 0.0f,
        p1.x, p1.y, 0.0f,
        p2.x, p2.y, 0.0f,
        p3.x, p3.y, 0.0f
    };
    out.indices = { 0,1,2, 0,2,3 };
    return out;
}