#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H


#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include<unordered_map>

std::string get_file_contents(const char* filename);


class Shader {
public:
    Shader(const char* vertPath, const char* fragPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& rhs) noexcept;
    Shader& operator=(Shader&& rhs) noexcept;

    void use() const { glUseProgram(id_); }

    void setVec4(const std::string& n, float x, float y, float z, float w) const {
        glUniform4f(loc(n), x, y, z, w);
    }
    void setFloat(const std::string& n, float v) const {
        glUniform1f(loc(n), v);
    }
    void setInt(const std::string& n, int v) const {
        glUniform1i(loc(n), v);
    }

    [[nodiscard]] GLuint id() const noexcept { return id_; }

private:
    GLuint id_{ 0 };
    mutable std::unordered_map<std::string, GLint> cache_;

    GLint loc(const std::string& n) const;   
    GLuint compile(GLenum type, const char* src);
};


#endif SHADER_CLASS_H