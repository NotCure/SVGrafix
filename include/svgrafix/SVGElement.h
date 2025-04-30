#pragma once




#include <string>
#include <unordered_map>
#include "Utility.h"




class SVGElement {
public:
    virtual ~SVGElement() = default;
    virtual void draw() = 0;

    void parseAttributes(const std::string& tagBody) {
        Utility::Parser::parseAttributes(tagBody, attributes_);
    }

    const std::string& getAttr(const std::string& name) const {
        static const std::string empty{};
        auto it = attributes_.find(name);
        return it != attributes_.end() ? it->second : empty;
    }

    const std::unordered_map<std::string, std::string>& getAllAttributes() const {
        return attributes_;
    }


protected:
    const std::unordered_map<std::string, std::string>& attributes() const {
        return attributes_;
    }

private:
    std::unordered_map<std::string, std::string> attributes_;
};



class SVG : public SVGElement {
    public:
	SVG() = default;
	~SVG() override = default;

	void draw() override {
	}


};


class SVGRect : public SVGElement {
public:
    SVGRect() = default;
    ~SVGRect() override = default;

    void draw() override {
    }
};	
