#pragma once
#include <glad/glad.h>

class ProgramShader
{
public:
	GLuint id;

	ProgramShader(GLuint shader1, GLuint shader2, GLuint shader3);
	ProgramShader();
	ProgramShader(GLuint shader1, GLuint shader2);
	ProgramShader(GLuint shader1);
	~ProgramShader();

	void use() const;

	operator GLuint() const;
};

