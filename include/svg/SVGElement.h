#pragma once

#include "../Utility/Parser.h"
#include<glad/glad.h>
#include <string>
#include <unordered_map>




struct TessOutput {
    std::vector<float>  verts;   
    std::vector<GLuint> indices; 
};

class SVGElement {
public:
    virtual ~SVGElement() = default;
    
    virtual TessOutput tessellate() const = 0;

    void parseAttributes(std::string_view body) {
        utility::parser::parse_attributes(body, attributes_);
    }

    std::string_view get_attr(std::string_view name) const {
        auto it = attributes_.find(std::string(name));
        return (it == attributes_.end())
            ? std::string_view{}
        : std::string_view{ it->second };
    }

    const std::unordered_map<std::string, std::string>& getAttributes() const {
        return attributes_;
    }

    [[nodiscard]]
    virtual std::string_view tag() const noexcept = 0;
private:
    std::unordered_map<std::string, std::string> attributes_;

};



class SVG : public SVGElement {
    public:
	SVG() = default;
	~SVG() override = default;

    TessOutput tessellate() const override;

    std::string_view tag() const noexcept override { return "svg"; }
};


struct RectGeom { float x, y, w, h; };
class SVGRect : public SVGElement {
public:
    SVGRect() = default;
    ~SVGRect() override = default;

    RectGeom geometry() const {
        auto num = [](std::string_view sv) { return std::stof(std::string(sv)); };
        return { num(get_attr("x")),
                 num(get_attr("y")),
                 num(get_attr("width")),
                 num(get_attr("height")) };
    }

    TessOutput tessellate() const override;

    std::string_view tag() const noexcept override { return "rect"; }
};	


class SVGGroup : public SVGElement {
public:
    SVGGroup() = default;
    ~SVGGroup() override = default;

    TessOutput tessellate() const override;

    std::string_view tag() const noexcept override { return "g"; }
};
