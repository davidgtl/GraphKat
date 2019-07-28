#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

class PointModel
{
public:

	unsigned int vao, vbo_pos, vbo_cols, vbo_uvs, width, height;

    PointModel();
    PointModel(int width, int height);
	~PointModel();

	void draw();
	void update(float tx, float ty);
    void updateSize(int width, int height);


private:
    GLfloat *colors;
};

