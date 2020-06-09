//
// Created by david on 21/11/2019.
//

#pragma once

#include <string>
#include <glad/glad.h>
#include <map>
#include <pugixml.hpp>
#include "ProgramShader.h"

using namespace std;

class ShaderLoader {
public:
    static void LoadShaders(const char *shaders_path);

    static map<string, ProgramShader> programMap;

private:
    static map<std::string, GLuint> s_mapShaderType;
    static map<string, Shader> shaderMap;

    static Shader resolveShader(pugi::xml_node shader_node);
};
