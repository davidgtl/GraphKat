#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

class PointModel
{
public:

	unsigned int vao, vbo_pos, vbo_cols, width, height;

    PointModel();
    PointModel(int width, int height);
	~PointModel();

	void draw();
	void update(float t);

private:
    GLfloat *colors;
};

