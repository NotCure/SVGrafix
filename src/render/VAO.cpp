#include <render/VAO.h>


VAO::VAO() { glGenVertexArrays(1, &id_); }

VAO::~VAO() { destroy(); }

VAO::VAO(VAO&& other) noexcept
    : id_{ std::exchange(other.id_, 0) } {}

VAO& VAO::operator=(VAO&& other) noexcept
{
    if (this != &other) {
        destroy();
        id_ = std::exchange(other.id_, 0);
    }
    return *this;
}

void VAO::bind()   const { glBindVertexArray(id_); }
void VAO::unbind() { glBindVertexArray(0); }

void VAO::addAttrib(GLuint index,
    GLint comps,
    GLenum type,
    GLsizei stride,
    const void* offset)
{
    glVertexAttribPointer(index, comps, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(index);
}

void VAO::destroy() noexcept
{
    if (id_) glDeleteVertexArrays(1, &id_);
    id_ = 0;
}