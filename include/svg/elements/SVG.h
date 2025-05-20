#pragma once
#include <svg/SVGElement.h>
#include <sstream>

struct ViewBox { float minX{}, minY{}, width{}, height{}; };



class SVG : public SVGElement {
public:
    SVG() = default;
    ~SVG() override = default;


    ViewBox viewbox() const;
    TessOutput tessellate() const override;

    std::string_view tag() const noexcept override { return "svg"; }
};
