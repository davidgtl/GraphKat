#include "ProgramShader.h"
#include "messages.h"

ProgramShader::ProgramShader(GLuint shader1, GLuint shader2, GLuint shader3)
{
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
ProgramShader::ProgramShader()
{
	id = -1;
}

ProgramShader::ProgramShader(GLuint shader1, GLuint shader2)
{
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
		fprintf(stderr, "Linker log:\n%s\n", log);
		fatal_error("");
	}
}

ProgramShader::ProgramShader(GLuint shader1)
{
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


ProgramShader::~ProgramShader()
{
	///glDeleteProgram(id);
}

void ProgramShader::use() const
{
	glUseProgram(id);
}

ProgramShader::operator GLuint() const
{
	return id;
}
