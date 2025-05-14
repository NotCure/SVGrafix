#include <svg/elements/Factory.h>
#include <svg/elements/SVGGroup.h>

REGISTER_SVG_ELEMENT("g", SVGGroup)

TessOutput SVGGroup::tessellate() const {
	return{};
}