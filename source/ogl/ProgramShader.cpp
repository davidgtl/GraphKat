#include "ProgramShader.h"
#include "dataflow/Context.h"
#include "messages.h"
#include <glm/glm.hpp>

/* Creates uniforms within the current context path */
ProgramShader::ProgramShader(const vector<Shader> &shaders) {
    id = glCreateProgram();

    for (auto &shader : shaders)
        glAttachShader(id, shader);

    glLinkProgram(id);

    int rvalue;

    glGetProgramiv(id, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking program shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(id, 10239, &length, log);
        fprintf(stderr, "Linker log:\n%s\n", log);
        fatal_error("");
    }

    struct uniform_info_t
    {
        GLint location;
        GLsizei count;
    };

    GLint uniform_count = 0;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniform_count);

    cout << "Hello Shadder!\n";
    if (uniform_count != 0)
    {
        GLint 	max_name_len = 0;
        GLsizei length = 0;
        GLsizei count = 0;
        GLenum 	type = GL_NONE;
        glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);



        /*
         * #define GL_FLOAT_VEC2 0x8B50
         * #define GL_FLOAT_VEC3 0x8B51
         * #define GL_FLOAT_VEC4 0x8B52
         * #define GL_INT_VEC2 0x8B53
         * #define GL_INT_VEC3 0x8B54
         * #define GL_INT_VEC4 0x8B55
         * #define GL_FLOAT_MAT2 0x8B5A
         * #define GL_FLOAT_MAT3 0x8B5B
         * #define GL_FLOAT_MAT4 0x8B5C
         * #define GL_INT 0x1404
         * #define GL_UNSIGNED_BYTE 0x1401
         * #define GL_SHORT 0x1402
         * #define GL_UNSIGNED_SHORT 0x1403
         * #define GL_INT 0x1404
         * #define GL_UNSIGNED_INT 0x1405
         * #define GL_FLOAT 0x1406
         * #define GL_BOOL 0x8B56
         *
         * #define GL_BOOL_VEC2 0x8B57
         * #define GL_BOOL_VEC3 0x8B58
         * #define GL_BOOL_VEC4 0x8B59
         */
        auto uniform_name = std::make_unique<char[]>(max_name_len);

        std::unordered_map<std::string, uniform_info_t> uniforms;

        //TODO: map them to the program shaders uniformtypes
        //based on that data do things whith the endpoints
        for (GLint i = 0; i < uniform_count; ++i)
        {
            glGetActiveUniform(id, i, max_name_len, &length, &count, &type, uniform_name.get());

            uniform_info_t uniform_info = {};
            uniform_info.location = glGetUniformLocation(id, uniform_name.get());
            uniform_info.count = count;

            printf("%s - %x#%d @%d\n", uniform_name.get(), type, uniform_info.count, uniform_info.location);
            UniformType utype = UniformType(type, uniform_info.count, uniform_info.location);

            Context::CurrentContext->createEndpoint(uniform_name.get(), utype);

            uniforms.emplace(std::make_pair(std::string(uniform_name.get(), length), uniform_info));
        }
    }
}

ProgramShader::ProgramShader(GLuint shader1, GLuint shader2, GLuint shader3){
    id = glCreateProgram();
    glAttachShader(id, shader1);
    glAttachShader(id, shader2);
    glAttachShader(id, shader3);
    glLinkProgram(id);

    int rvalue;

    glGetProgramiv(id, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking program shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(id, 10239, &length, log);
        fprintf(stderr, "Linker log:\n%s\n", log);
        fatal_error("");
    }
}

ProgramShader::ProgramShader() : dataContext("shader") {
    id = -1;
}

ProgramShader::ProgramShader(GLuint shader1, GLuint shader2) {
    id = glCreateProgram();
    glAttachShader(id, shader1);
    glAttachShader(id, shader2);
    glLinkProgram(id);

    int rvalue;

    glGetProgramiv(id, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking program shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(id, 10239, &length, log);
        glCheckError();
        fprintf(stderr, "Linker log:\n%s\n", log);

        fatal_error("");
    }
}

ProgramShader::ProgramShader(GLuint shader1) {
    id = glCreateProgram();
    glAttachShader(id, shader1);
    glLinkProgram(id);

    int rvalue;

    glGetProgramiv(id, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking program shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(id, 10239, &length, log);
        fprintf(stderr, "Linker log:\n%s\n", log);
        fatal_error("");
    }
}


ProgramShader::~ProgramShader() {
    ///glDeleteProgram(id);
}

void ProgramShader::use() const {
    glUseProgram(id);
}

ProgramShader::operator GLuint() const {
    return id;
}