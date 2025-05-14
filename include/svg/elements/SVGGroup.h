#pragma once
#include <svg/SVGElement.h>

class SVGGroup : public SVGElement {
public:
    SVGGroup() = default;
    ~SVGGroup() override = default;

    TessOutput tessellate() const override;

    std::string_view tag() const noexcept override { return "g"; }
};
