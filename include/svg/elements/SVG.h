#pragma once
#include <svg/SVGElement.h>
#include <sstream>
#include <render/SceneTypes.h>



class SVG : public SVGElement {
public:
    SVG() = default;
    ~SVG() override = default;


    ViewBox viewbox() const;
    TessOutput tessellate() const override;

    std::string_view tag() const noexcept override { return "svg"; }
};
