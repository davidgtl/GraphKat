#pragma once

#include <iostream>
#include <GLFW/glfw3.h>
#define glCheckError() glCheckError_(__FILE__, __LINE__)


inline void fatal_error()
{
	glfwTerminate();
	exit(-1);
}

inline void fatal_error(const char* message)
{
	std::cout << message << std::endl;
	glfwTerminate();
	exit(-1);
}

inline void glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		//case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		//case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		default:
			error = "Unknown error"; break;
		}
		printf("%s | %s (%d)\n", error.c_str(), file, line);
		fatal_error();
	}

}
