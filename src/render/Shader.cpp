
#include <render/Shader.h>


static void printLog(GLuint obj, PFNGLGETSHADERINFOLOGPROC getInfoLog, PFNGLGETSHADERSOURCEPROC /*unused*/) {
	GLint len = 0;
	getInfoLog(obj, 0, &len, nullptr);
	if (len > 0) {
		std::string log(len, '\0');
		getInfoLog(obj, len, nullptr, &log[0]);
		std::cerr << log << "\n";
	}
}

std::string get_file_contents(const char* filename) {
	std::ifstream in(filename, std::ios::in);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
    auto load = [&](const char* path) {
        std::ifstream in(path); std::string s((std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>());
        return s;
    };

    std::string vertCode = load(vertexFile);
    std::string fragCode = load(fragmentFile);
    const char* vsrc = vertCode.c_str();
    const char* fsrc = fragCode.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsrc, nullptr);
    glCompileShader(vs);
    GLint ok;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        std::cerr << "--- VERTEX SHADER COMPILE ERROR ---\n";
        printLog(vs, glGetShaderInfoLog, nullptr);
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsrc, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        std::cerr << "--- FRAGMENT SHADER COMPILE ERROR ---\n";
        printLog(fs, glGetShaderInfoLog, nullptr);
    }

    ID = glCreateProgram();
    glAttachShader(ID, vs);
    glAttachShader(ID, fs);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &ok);
    if (!ok) {
        std::cerr << "--- SHADER LINK ERROR ---\n";
        printLog(ID, glGetProgramInfoLog, nullptr);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Shader::Activate()
{
	glUseProgram(this->ID);
}
void Shader::Delete()
{
	glDeleteProgram(this->ID);
}

