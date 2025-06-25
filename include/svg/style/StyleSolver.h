#pragma once
#include <svg/style/Css.h>


struct ComputedStyle {
	std::optional<Rgba>    fill;
	std::optional<Rgba>    stroke;
	std::optional<float>   stroke_width;
	std::optional<Mat3>    transform;
};

class StyleSolver
{
public:
	StyleSolver(css::StyleSheet const& sheet);

	ComputedStyle resolve(SVGElement const& el) const;

private:
	css::StyleSheet const& sheet_;
};

