
#include <render/Shader.h>
#include <fstream>
#include <sstream>
#include <iostream>


static std::string loadFile(const char* path) {
    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f) throw std::runtime_error(std::string("Cannot open ") + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

GLuint Shader::compile(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(s, len, nullptr, log.data());
        std::cerr << log << '\n';
        glDeleteShader(s);
        throw std::runtime_error("Shader compile failed");
    }
    return s;
}

Shader::Shader(const char* vertPath, const char* fragPath)
{
    std::string vsrc = loadFile(vertPath);
    std::string fsrc = loadFile(fragPath);

    GLuint vs = compile(GL_VERTEX_SHADER, vsrc.c_str());
    GLuint fs = compile(GL_FRAGMENT_SHADER, fsrc.c_str());

    id_ = glCreateProgram();
    glAttachShader(id_, vs);
    glAttachShader(id_, fs);
    glLinkProgram(id_);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok; glGetProgramiv(id_, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len; glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(id_, len, nullptr, log.data());
        std::cerr << log << '\n';
        glDeleteProgram(id_);
        throw std::runtime_error("Program link failed");
    }
}

Shader::~Shader() { if (id_) glDeleteProgram(id_); }

Shader::Shader(Shader&& rhs) noexcept
    : id_{ std::exchange(rhs.id_, 0) }, cache_{ std::move(rhs.cache_) } {}

Shader& Shader::operator=(Shader&& rhs) noexcept {
    if (this != &rhs) {
        if (id_) glDeleteProgram(id_);
        id_ = std::exchange(rhs.id_, 0);
        cache_ = std::move(rhs.cache_);
    }
    return *this;
}

GLint Shader::loc(const std::string& n) const {
    if (auto it = cache_.find(n); it != cache_.end())
        return it->second;
    GLint l = glGetUniformLocation(id_, n.c_str());
    cache_[n] = l;
    return l;
}

