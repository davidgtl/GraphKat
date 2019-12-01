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
#include <boost/regex.hpp>

using namespace std;

map<string, GLuint> ShaderLoader::s_mapShaderType =
        {
                {"vert", Shader::VERTEX_SHADER},
                {"frag", Shader::FRAGMENT_SHADER},
                {"comp", Shader::COMPUTE_SHADER},
                {"geom", Shader::GEOMETRY_SHADER},
        };


map<string, Shader> ShaderLoader::shaderMap = {};
map<string, ProgramShader> ShaderLoader::programMap = {};

Shader ShaderLoader::resolveShader(pugi::xml_node shader_node) {
    auto a_ref = shader_node.attribute("ref");
    auto a_src = shader_node.attribute("src");
    auto a_name = shader_node.attribute("name");
    if (a_ref != NULL) {
        if (shaderMap.count(a_ref.value())) {
            return shaderMap[a_ref.value()];
        }
        fatal_error("Could not load referenced shader");
    } else if (a_src != NULL) {
        string src = a_src.value();
        GLuint type = s_mapShaderType[src.substr(src.length(  ) - 4, 4)];

        ifstream t(src);

        string str((istreambuf_iterator<char>(t)),
                   istreambuf_iterator<char>());
        //string str = "uniform vec2 model;\nuniform int tex;\nint main(void){\nprintf(hello world)\n}";

        const char *unipat = R"(uniform (.*?) (.*?);)";
        const char *inpat = R"(in (.*) (.*);)";
        boost::regex unireg(unipat), inreg(inpat);

        /*boost::smatch what;
        while (boost::regex_search(str, what, unireg))
        {
            what.
            //std::cout << what[0] << '\n';
            std::cout << what[1] << "_" << what[2] << '\n';
        }*/
        boost::sregex_token_iterator iter(str.begin(), str.end(), unireg, {1, 2});
        boost::sregex_token_iterator end;

        for (; iter != end; ++iter) {
            std::cout << *iter << '\n';
        }

        if (a_name != NULL) {
            shaderMap[a_name.value()] = Shader(src, type);
            return shaderMap[a_name.value()];
        } else {
            shaderMap[src] = Shader(src, type);
            return shaderMap[src];
        }
    }
    fatal_error("Could not load shader");
}


map<string, ProgramShader> ShaderLoader::LoadShaders(const char *shaders_path) {
    pugi::xml_document doc;

    ifstream stream(shaders_path);
    doc.load(stream);

    pugi::xpath_node_set xpath_shaders = doc.select_nodes("/shaders/shaders/shader");

    for (auto xpath_shader : xpath_shaders)
        resolveShader(xpath_shader.node());


    pugi::xpath_node_set xpath_programs = doc.select_nodes("/shaders/programs/program");

    for (auto xpath_program : xpath_programs) {
        pugi::xml_node xml_program = xpath_program.node();
        string name = xml_program.attribute("name").value();

        vector<Shader> shaders;
        auto xml_shaders = xml_program.children("shader");

        for (auto node : xml_shaders)
            shaders.push_back(resolveShader(node));

        programMap[name] = ProgramShader(shaders);
        glCheckError();

    }

    return programMap;
}
