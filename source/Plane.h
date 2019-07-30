#pragma once
#include "PointModel.h"
#include <glm/glm.hpp>

using namespace glm;

class Plane
{
public:
	unsigned int vao, vbo, ebo, uvs;

    vec2 origin, size;
    float z;

    Plane(bool invertY = false);

    Plane(vec2 origin, vec2 size, float z, bool invertY = false);
	~Plane();

	void draw();

private:
    void updateVetices();

    void init(vec2 origin, vec2 size, float z, bool invertY = false);
};

