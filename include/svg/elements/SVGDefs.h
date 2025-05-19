#pragma once
#include <svg/SVGElement.h>

class SVGDefs : public SVGElement {
public:
    TessOutput tessellate() const override;
    std::string_view tag() const noexcept override { return "defs"; }
};