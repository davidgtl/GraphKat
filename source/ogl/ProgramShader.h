#pragma once

#include <glad/glad.h>
#include "Shader.h"
#include "UniformType.h"
#include <vector>
#include "nodeprims/Shaders.h"

using namespace std;

class ProgramShader {
public:

    GLuint id;
    std::unordered_map<std::string, UniformType> uniforms;

    ProgramShader(GLuint shader1, GLuint shader2, GLuint shader3);

    ProgramShader();

    ProgramShader(GLuint shader1, GLuint shader2);

    ProgramShader(GLuint shader1);

    ProgramShader(const vector<Shader> &shaders);

    ~ProgramShader();

    void use() const;

    operator GLuint() const;

    template<typename T>
    void setUniform(const string &name, T value) {
        //assert(uniforms.find(name) != uniforms.end());
        if(uniforms.find(name) != uniforms.end())
            Shaders::bind_uniform(uniforms[name], value);
    }
};

