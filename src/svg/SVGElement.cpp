#include <svg/SVGElement.h>
#include <array> 

TessOutput SVG::tessellate() const {

	auto num = [](std::string_view sv) { return std::stof(std::string(sv)); };

	float x = num(get_attr("x"));        
	float y = num(get_attr("y"));      
	float w = num(get_attr("width"));    
	float h = num(get_attr("height"));  

	std::array<float, 8> v = { x, y + h, x + w, y + h, x + w, y, x, y };
	std::array<GLuint, 6> i = { 0,1,2, 0,2,3 };

	return { std::vector<float>(v.begin(), v.end()), std::vector<GLuint>(i.begin(), i.end()) };
}


TessOutput SVGRect::tessellate() const {


	return TessOutput{};
}



TessOutput SVGGroup::tessellate() const {


	return TessOutput{};
}