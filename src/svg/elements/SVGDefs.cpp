#include <svg/elements/Factory.h>
#include <svg/elements/SVGDefs.h>

REGISTER_SVG_ELEMENT("defs", SVGDefs)

TessOutput SVGDefs::tessellate() const {
	return{};
}