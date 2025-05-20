// scene/SceneBuilder.cpp

#include <render/SceneBuilder.h>
#include <svg/elements/SVGRect.h>  

void SceneBuilder::build() {
    buckets_.clear();
    std::unordered_map<std::string, std::string> empty;
    walk(dom_.root(), empty);

    for (auto& [color, bucket] : buckets_) {
        GLuint vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
            bucket.verts.size() * sizeof(float),
            bucket.verts.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            bucket.indices.size() * sizeof(GLuint),
            bucket.indices.data(), GL_STATIC_DRAW);

        // assume layout(location=0) is vec3 position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof(float), (void*)0);

        glBindVertexArray(0);

        vaos_.push_back(vao);
        vbos_.push_back(vbo);
        ibos_.push_back(ebo);

        bucket.color = color;
    }
}

void SceneBuilder::walk(
    const SVGNode* node,
    const std::unordered_map<std::string, std::string>& inherited)
{
    auto myAttrs = dom_.stylesheet().declartions_for(*node->element);
    auto style = inherited;
    style.insert(myAttrs.begin(), myAttrs.end());

    if (auto* rect = dynamic_cast<SVGRect*>(node->element.get())) {
        auto mesh = rect->tessellate();
        Rgba col = parse_color(style["fill"]);
        auto& bucket = buckets_[col];
        bucket.color = col;

        GLuint base = bucket.verts.size() / 3;
        bucket.verts.insert(bucket.verts.end(),
            mesh.verts.begin(),
            mesh.verts.end());
        for (auto idx : mesh.indices)
            bucket.indices.push_back(base + idx);
    }

    for (auto& child : node->children)
        walk(child.get(), style);
}
