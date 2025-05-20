#include <svg/Css.h>
#include <svg/DomSVG.h>
#include <utility/Color.h>

struct MeshBucket {
    std::vector<float>  verts;   
    std::vector<GLuint> indices; 
    Rgba                color;   

};
class SceneBuilder {
    public:
        explicit SceneBuilder(const DomSVG& dom) : dom_(dom) {};

        void build();

        void draw() const;

    private:
        void walk(const SVGNode* node,
            const std::unordered_map<std::string, std::string>& inheritedStyle);

        const DomSVG& dom_;
        std::unordered_map<Rgba, MeshBucket> buckets_;
        std::vector<GLuint> vbos_, ibos_, vaos_;
};