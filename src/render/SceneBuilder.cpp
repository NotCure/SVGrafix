// scene/SceneBuilder.cpp

#include <render/SceneBuilder.h>
#include <svg/elements/SVGRect.h>  
#include <render/EBO.h>
#include <render/VBO.h>
#include <render/VAO.h>


void SceneBuilder::build() {
    buckets_.clear();
    vaos_.clear();
    vbos_.clear();
    ebos_.clear();
    indexCounts_.clear();
    fills_.clear();


    RenderContext ctx;
    for (auto const& childNode : dom_.root()->children) {
        if (auto* svgEl = dynamic_cast<SVG*>(childNode->element.get())) {
            ctx.vb = svgEl->viewbox();
            break;
        }
    }

    walk(dom_.root(), ctx);


    for (auto& [color, bucket] : buckets_) {
        VAO vao;
        VBO vbo(bucket.verts.data(),
            bucket.verts.size() * sizeof(float));
        EBO ebo(bucket.indices.data(),
            bucket.indices.size() * sizeof(GLuint));\


        vao.Bind();
        vbo.Bind();
        ebo.Bind();

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        vao.Unbind();
        vbo.Unbind();
        ebo.Unbind();

        vaos_.push_back(vao.ID);
        vbos_.push_back(vbo.ID);
        ebos_.push_back(ebo.ID);
        indexCounts_.push_back((GLsizei)bucket.indices.size());
        fills_.push_back(color);
    }

    std::cerr << "Got " << buckets_.size() << " color buckets\n";
    for (auto const& [c, bucket] : buckets_) {
        std::cerr << "  color=("
            << c.r << "," << c.g << "," << c.b << "," << c.a << ") "
            << bucket.verts.size() / 3 << " verts, "
            << bucket.indices.size() << " indices\n";
    }
}

void SceneBuilder::walk(const SVGNode* node, RenderContext ctx) {
    auto local = dom_.stylesheet().declartions_for(*node->element);
    ctx.style.insert(local.begin(), local.end());

    if (auto* rect = dynamic_cast<SVGRect*>(node->element.get())) {
        auto mesh = rect->tessellate();

        for (size_t i = 0; i < mesh.verts.size(); i += 3) {
            Vec3 P{ mesh.verts[i + 0],
                    mesh.verts[i + 1],
                    1.0f };
            Vec3 R = ctx.ctm * P;
            mesh.verts[i + 0] = R.x;
            mesh.verts[i + 1] = R.y;
        }

        for (size_t i = 0; i < mesh.verts.size(); i += 3) {
            float x = mesh.verts[i],
                y = mesh.verts[i + 1];
            mesh.verts[i] = ((x - ctx.vb.minX) / ctx.vb.width) * 2.f - 1.f;
            mesh.verts[i + 1] = 1.f - ((y - ctx.vb.minY) / ctx.vb.height) * 2.f;
        }

		std::cerr << "css color" << ctx.style["fill"] << "\n";
        Rgba fill = parse_color(ctx.style["fill"]);
        auto& B = buckets_[fill];
        B.color = fill;

        GLuint base = (GLuint)(B.verts.size() / 3);
        B.verts.insert(B.verts.end(),
            mesh.verts.begin(), mesh.verts.end());
        for (auto idx : mesh.indices)
            B.indices.push_back(base + idx);
    }

    for (auto& child : node->children)
        walk(child.get(), ctx);
}


void SceneBuilder::draw( Shader& shader) const {
    shader.Activate();
    for (size_t i = 0; i < vaos_.size(); ++i) {
        const auto& c = fills_[i];
        shader.set_vec4("uFill", c.r, c.g, c.b, c.a);
        glBindVertexArray(vaos_[i]);

        glDrawElements(GL_TRIANGLES, indexCounts_[i], GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
}
