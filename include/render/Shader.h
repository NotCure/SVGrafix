#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H


#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>


std::string get_file_contents(const char* filename);


class Shader {
public:
	GLuint ID;
	Shader(const char* variable, const char* fragmentfile);

	void Activate();
	void Delete();



    void set_float(const std::string& name, float v) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), v);
    }
    void set_vec4(const std::string& name,
        float x, float y, float z, float w) const {

        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1) {
            glUniform4f(loc, x, y, z, w);
        }
    }
    void set_int(const std::string& name, int v) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), v);
    }

};


#endif SHADER_CLASS_H