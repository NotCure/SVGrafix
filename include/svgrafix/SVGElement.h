#pragma once

#include "../Utility/Parser.h"

#include <string>
#include <unordered_map>





class SVGElement {
public:
    virtual ~SVGElement() = default;
    virtual void draw() = 0;

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

	void draw() override {
	}

    std::string_view tag() const noexcept override { return "svg"; }
};


struct RectGeom { float x, y, w, h; };
class SVGRect : public SVGElement {
public:
    SVGRect() = default;
    ~SVGRect() override = default;

    RectGeom geometry(float svgH) const {         
        float x = std::stof(std::string(get_attr("x")));
        float y = std::stof(std::string(get_attr("y")));
        float w = std::stof(std::string(get_attr("width")));
        float h = std::stof(std::string(get_attr("height")));
        return { x, svgH - y - h, w, h };
    }
    std::string_view tag() const noexcept override { return "rect"; }
};	


class SVGGroup : public SVGElement {
public:
    SVGGroup() = default;
    ~SVGGroup() override = default;

    void draw() override {
    }
    std::string_view tag() const noexcept override { return "g"; }
};
