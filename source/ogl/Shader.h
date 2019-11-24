#pragma once

#include <glad/glad.h>
#include <string>


class Shader {
public:
    static const GLuint VERTEX_SHADER;
    static const GLuint FRAGMENT_SHADER;
    static const GLuint COMPUTE_SHADER;
    static const GLuint GEOMETRY_SHADER;
    GLuint id;
    const char *filepath;

    Shader();
    Shader(const char *filepath, GLint shaderType);
    Shader(const std::string& filepath, GLint shaderType);

    ~Shader();

    operator GLuint() const;
};
