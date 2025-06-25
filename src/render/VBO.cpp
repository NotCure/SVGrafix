#include <render/VBO.h>
#include <utility>  

VBO::VBO() = default;

VBO::VBO(const GLfloat* data, GLsizeiptr size, GLenum usage)
{
    glGenBuffers(1, &id_);
    glBindBuffer(GL_ARRAY_BUFFER, id_);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

VBO::~VBO() { destroy(); }

VBO::VBO(VBO&& other) noexcept
    : id_{ std::exchange(other.id_, 0) } {}

VBO& VBO::operator=(VBO&& other) noexcept
{
    if (this != &other) {
        destroy();
        id_ = std::exchange(other.id_, 0);
    }
    return *this;
}

void VBO::bind() const { glBindBuffer(GL_ARRAY_BUFFER, id_); }
void VBO::unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VBO::destroy() noexcept
{
    if (id_) glDeleteBuffers(1, &id_);
    id_ = 0;
}