#include <svg/elements/Factory.h>
#include <svg/elements/SVG.h>

REGISTER_SVG_ELEMENT("svg", SVG)

TessOutput SVG::tessellate() const {
	return{};
}