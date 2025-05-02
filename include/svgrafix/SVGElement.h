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


class SVGRect : public SVGElement {
public:
    SVGRect() = default;
    ~SVGRect() override = default;

    void draw() override {
    }
    std::string_view tag() const noexcept override { return "rect"; }
};	
