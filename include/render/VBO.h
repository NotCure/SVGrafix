#ifndef VBO_CLASS_H
#define VBO_CLASS_H
#include<glad/glad.h>
#include <utility> 
#include <span>

class VBO
{
public:
    VBO();                                                   
    VBO(const GLfloat* data, GLsizeiptr size,
        GLenum usage = GL_STATIC_DRAW);

    template<typename T>                                     
    explicit VBO(std::span<const T> bytes,
        GLenum usage = GL_STATIC_DRAW)
        : VBO(reinterpret_cast<const GLfloat*>(bytes.data()),
            static_cast<GLsizeiptr>(bytes.size_bytes()),
            usage) {}

    ~VBO();                                                    

    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;
    VBO(VBO&& other) noexcept;
    VBO& operator=(VBO&& other) noexcept;

    void   bind()   const;
    static void unbind();

    [[nodiscard]] GLuint id() const noexcept { return id_; }

private:
    GLuint id_{ 0 };

    void destroy() noexcept;
};







#endif