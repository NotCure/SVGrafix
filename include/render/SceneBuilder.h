#include <svg/Css.h>
#include <svg/DomSVG.h>
#include <utility/Color.h>
#include <render/Shader.h>
#include <utility/Mat3.h>
#include <svg/elements/SVG.h>



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

class SceneBuilder {
    public:
        explicit SceneBuilder(const DomSVG& dom) : dom_(dom) {};

        void build();

        void draw( Shader& shader) const;

    private:

        void walk(const SVGNode* node, RenderContext ctx);

        const DomSVG& dom_;
        std::unordered_map<Rgba, MeshBucket> buckets_;

        std::vector<GLuint>  vaos_, vbos_, ebos_;
        std::vector<GLsizei> indexCounts_;
        std::vector<Rgba>   fills_;

};