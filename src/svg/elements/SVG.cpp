#include <svg/elements/Factory.h>
#include <svg/elements/SVG.h>


REGISTER_SVG_ELEMENT("svg", SVG)

ViewBox SVG::viewbox() const {
    auto s = get_attr("viewBox");
    std::stringstream ss{ std::string(s) }; 
    ViewBox vb{};
    ss >> vb.minX >> vb.minY >> vb.width >> vb.height;
    return vb;
}

TessOutput SVG::tessellate() const {
	return{};
}

