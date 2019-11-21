//
// Created by david on 21/11/2019.
//

#include "ShaderLoader.h"
#include "Shader.h"
#include "ProgramShader.h"
#include <pugixml.hpp>
#include <iostream>
#include <fstream>
#include <messages.h>
#include <map>
#include <vector>
#include <unistd.h>

using namespace std;

map<string, GLuint > ShaderLoader::s_mapShaderType =
        {
                { "vert", Shader::VERTEX_SHADER },
                { "frag", Shader::FRAGMENT_SHADER },
                { "comp", Shader::COMPUTE_SHADER },
                { "geom", Shader::GEOMETRY_SHADER },
        };


map<string, ProgramShader> ShaderLoader::LoadShaders(const char *shaders_path) {
    pugi::xml_document doc;

    ifstream stream(shaders_path);
    doc.load(stream);
    map<string, ProgramShader> programMap;


    pugi::xpath_node_set xpath_programs = doc.select_nodes("/shaders/program");

    for (auto xpath_program : xpath_programs)
    {
        pugi::xml_node xml_program = xpath_program.node();
        string name = xml_program.attribute("name").value();

        vector<Shader> shaders;
        auto xml_shaders = xml_program.children("shader");

        for (auto node : xml_shaders) {
            string src  = node.attribute("src").value();

            GLuint type = s_mapShaderType[src.substr(src.length() - 4, 4)];
            shaders.emplace_back(src, type);
        }

        programMap[name] = ProgramShader(shaders);
        glCheckError();

    }

    return programMap;
}
