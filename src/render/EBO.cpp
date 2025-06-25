#include <render/EBO.h>

EBO::EBO() = default;

EBO::~EBO() { destroy(); }

EBO::EBO(EBO&& other) noexcept
    : id_{ std::exchange(other.id_, 0) } {}

EBO& EBO::operator=(EBO&& other) noexcept
{
    if (this != &other) {
        destroy();
        id_ = std::exchange(other.id_, 0);
    }
    return *this;
}


void EBO::bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_); }
void EBO::unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

void EBO::destroy() noexcept
{
    if (id_) glDeleteBuffers(1, &id_);
    id_ = 0;
}