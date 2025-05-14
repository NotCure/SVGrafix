#pragma once

#include "../Utility/Parser.h"
#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <vector>


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




