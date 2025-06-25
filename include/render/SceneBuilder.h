#include <svg/Css.h>
#include <svg/DomSVG.h>
#include <utility/Color.h>
#include <render/Shader.h>
#include <utility/Mat3.h>
#include <svg/elements/SVG.h>
#include <render/SceneTypes.h>

#include <render/VBO.h>
#include <render/EBO.h>
#include <render/VAO.h> 



struct GPUBatch          
{
    VAO     vao;
    VBO     vbo;
    EBO     ebo;
    GLsizei indexCount;
    Rgba    fill;        
};

class SceneBuilder {
public:

    explicit SceneBuilder(const DomSVG& dom) : dom_(dom) {}

    void build();

    void draw(Shader& shader) const;

private:
    void walk(const SVGNode* node, RenderContext ctx);

    const DomSVG& dom_;

    std::unordered_map<Rgba, MeshBucket> buckets_;  

    std::vector<GPUBatch> batches_;                  
};

