#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include<glad/glad.h>
#include <utility>
#include <span>

class EBO
{
public:
    EBO();                                   

  
    template<typename T>
    explicit EBO(std::span<const T> indices,
        GLenum usage = GL_STATIC_DRAW)
    {
        glGenBuffers(1, &id_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(indices.size_bytes()),
            indices.data(),
            usage);
    }

    ~EBO();

    EBO(const EBO&) = delete;
    EBO& operator=(const EBO&) = delete;
    EBO(EBO&& other) noexcept;
    EBO& operator=(EBO&& other) noexcept;

    void   bind()   const;
    static void unbind();

    [[nodiscard]] GLuint id() const noexcept { return id_; }

private:
    GLuint id_{ 0 };
    void destroy() noexcept;
};

#endif