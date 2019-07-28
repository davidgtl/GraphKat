#include "Plane.h"
#include <glad/glad.h>


Plane::Plane()
{
    init(vec2(0, 0), vec2(1,1), 0);
}


Plane::Plane(vec2 origin, vec2 size, float z) {
    init(origin, size, z);
}

void Plane::init(vec2 origin, vec2 size, float z) {
    this->origin = origin;
    this->size = size;
    this->z = z;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &uvs);


    glBindVertexArray(vao);

    updateVetices();

    int indices[] = {
            0,1,2,
            2,1,3
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, uvs);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    float uv[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), uv, GL_STATIC_DRAW);
}


Plane::~Plane()
{
}

void Plane::draw()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}

void Plane::updateVetices(){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vec2 ostart = vec2(-1, -1);
    vec2 oend = vec2(1, 1);

    vec2 start = ostart + origin * (oend - ostart);
    vec2 end = start + size * (oend - ostart);


    float vertices[] = {
            start.x, start.y, z,
            end.x, start.y, z,
            start.x, end.y, z,
            end.x, end.y, z
    };

    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
}
