#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include"VBO.h"


class VAO
{
public:
    VAO();                                   
    ~VAO();                                 

    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;

    VAO(VAO&& other) noexcept;               
    VAO& operator=(VAO&& other) noexcept;   

    void   bind()   const;
    static void unbind();

    void addAttrib(GLuint index,
        GLint  comps,
        GLenum type,
        GLsizei stride,
        const void* offset);

    [[nodiscard]] GLuint id() const noexcept { return id_; }

private:
    GLuint id_{ 0 };
    void   destroy() noexcept;
};

#endif