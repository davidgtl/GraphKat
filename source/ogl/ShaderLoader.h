//
// Created by david on 21/11/2019.
//

#pragma once

#include <string>
#include <glad/glad.h>
#include <map>
#include "ProgramShader.h"

using namespace std;

class ShaderLoader {
public:
    static map<string, ProgramShader> LoadShaders(const char *shaders_path);
private:
    static map<std::string, GLuint> s_mapShaderType;
};