#pragma once
#include <svg/SVGElement.h>


class SVG : public SVGElement {
public:
    SVG() = default;
    ~SVG() override = default;

    TessOutput tessellate() const override;

    std::string_view tag() const noexcept override { return "svg"; }
};
