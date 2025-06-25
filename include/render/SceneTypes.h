#pragma once
#include <utility/Mat3.h>
#include <utility/Color.h>
#include <glad/glad.h>

struct ViewBox { float minX{}, minY{}, width{}, height{}; };
          

struct RenderContext {
    ViewBox vb = { 0,0,1,1 };
    Mat3 ctm = Mat3::identity();
    std::unordered_map<std::string, std::string> style;
};

struct MeshBucket {
    std::vector<float>  verts;
    std::vector<GLuint> indices;
    Rgba                color;

};