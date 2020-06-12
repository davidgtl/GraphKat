#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtx/string_cast.hpp>

#define glCheckError() glCheckError_(__FILE__, __LINE__)


template<glm::length_t L, typename T, glm::qualifier Q>
std::ostream &operator<<(std::ostream &out, const glm::vec<L, T, Q> &g) {
    return out << glm::to_string(g);
}

template<class... Args>
std::string format(Args &&... args) {
    std::ostringstream ostr;

    (ostr << ... << args);

    return ostr.str();
}

inline void fatal_error() {
    glfwTerminate();
    exit(-1);
}

inline void fatal_error(const char *message) {
    std::cout << message << std::endl;
    glfwTerminate();
    exit(-1);
}

inline void glCheckError_(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
                //case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
                //case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                error = "Unknown error";
                break;
        }
        printf("%s | %s (%d)\n", error.c_str(), file, line);
        fatal_error();
    }

}
