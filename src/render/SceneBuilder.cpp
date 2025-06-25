// scene/SceneBuilder.cpp

#include <render/SceneBuilder.h>
#include <svg/elements/SVGRect.h>  



void SceneBuilder::build()
{
    buckets_.clear();
    batches_.clear();          // RAII wrappers auto-delete here

    // ------- build CPU buckets exactly as before -------------------------
    RenderContext ctx;
    for (auto const& child : dom_.root()->children)
        if (auto* svg = dynamic_cast<SVG*>(child->element.get())) {
            ctx.vb = svg->viewbox();
            break;
        }
    walk(dom_.root(), ctx);
    // ---------------------------------------------------------------------

    // ------- upload each bucket to the GPU (now RAII) --------------------
    for (auto& [color, bucket] : buckets_)
    {
        // create GPU buffers (constructors upload data)
        VBO vbo(bucket.verts.data(),
            bucket.verts.size() * sizeof(float));

        EBO ebo{ std::span<const GLuint>(bucket.indices) };

        VAO vao;
        vao.bind();
        vbo.bind();
        ebo.bind();
        vao.addAttrib(0, 3, GL_FLOAT, 0, nullptr);   // helper in new VAO
        VAO::unbind();

        batches_.push_back(GPUBatch{
            std::move(vao),
            std::move(vbo),
            std::move(ebo),
            static_cast<GLsizei>(bucket.indices.size()),
            color
            });
    }

    std::cerr << "Got " << batches_.size() << " color buckets\n";
    for (auto const& b : batches_)
        std::cerr << "  color=("
        << b.fill.r << ',' << b.fill.g << ','
        << b.fill.b << ',' << b.fill.a << ") "
        << b.indexCount / 3 << " verts, "
        << b.indexCount << " indices\n";
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
    shader.use();                      
    for (auto const& b : batches_) {
        shader.setVec4("uFill", b.fill.r, b.fill.g, b.fill.b, b.fill.a);
        b.vao.bind();
        glDrawElements(GL_TRIANGLES, b.indexCount, GL_UNSIGNED_INT, nullptr);
    }
    VAO::unbind();
}
