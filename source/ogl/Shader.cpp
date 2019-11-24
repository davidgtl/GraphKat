#include "Shader.h"
#include "io.h"
#include "messages.h"
#include <sys/types.h>

const GLuint Shader::VERTEX_SHADER = GL_VERTEX_SHADER;
const GLuint Shader::FRAGMENT_SHADER = GL_FRAGMENT_SHADER;
const GLuint Shader::COMPUTE_SHADER = GL_COMPUTE_SHADER;
const GLuint Shader::GEOMETRY_SHADER = GL_GEOMETRY_SHADER;

Shader::Shader(const std::string& filepath, GLint shaderType) : Shader(filepath.c_str(), shaderType) {}

Shader::Shader(const char *filepath, GLint shaderType) {
    this->filepath = filepath;
    id = glCreateShader(shaderType);
    ifstream t(filepath);

    string str((istreambuf_iterator<char>(t)),
               istreambuf_iterator<char>());
    const GLchar *source = str.c_str();

    if (source == NULL) {
        printf("Could not open shader file: %s", filepath);
        fatal_error("");
    }

    glShaderSource(id, 1, &source, NULL);

    glCompileShader(id);

    int rvalue;
    glGetShaderiv(id, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        printf("Error in compiling shader %s\n", filepath);
        GLchar log[10240];
        GLsizei length;
        glGetShaderInfoLog(id, 10239, &length, log);
        printf("Compiler log:\n%s\n", log);
        glCheckError();
        fatal_error("");
    }
}


Shader::~Shader() {
    //glDeleteShader(id);
}

Shader::operator GLuint() const {
    return id;
}

Shader::Shader():id(-1) {
}
