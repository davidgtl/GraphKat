#pragma once

#include <glad/glad.h>
#include "Shader.h"
#include "UniformType.h"
#include <vector>
#include <dataflow/Context.h>

using namespace std;

class ProgramShader {
public:

    Context dataContext;
    GLuint id;

    ProgramShader(GLuint shader1, GLuint shader2, GLuint shader3);

    ProgramShader();

    ProgramShader(GLuint shader1, GLuint shader2);

    ProgramShader(GLuint shader1);

    ProgramShader(const vector<Shader> &shaders);

    ~ProgramShader();

    void use() const;

    operator GLuint() const;

    void setUniform();
};

