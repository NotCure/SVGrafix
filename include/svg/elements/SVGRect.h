#pragma once
#include <svg/SVGElement.h>


struct RectGeom { float x, y, w, h; };
class SVGRect : public SVGElement {
public:
    SVGRect() = default;
    ~SVGRect() override = default;

    RectGeom geometry() const;
    TessOutput tessellate() const override;

    std::string_view tag() const noexcept override { return "rect"; }
};