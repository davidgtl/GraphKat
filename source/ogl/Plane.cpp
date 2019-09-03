#include "Plane.h"
#include <glad/glad.h>


Plane::Plane(float zindex, bool invertY) {
    init(vec2(0, 0), vec2(1, 1), zindex, invertY);
}


Plane::Plane(vec2 origin, vec2 size, float z, bool invertY) {
    init(origin, size, z, invertY);
}

void Plane::init(vec2 origin, vec2 size, float z, bool invertY) {
    this->origin = origin;
    this->size = size;
    this->z = z;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &uvs);


    glBindVertexArray(vao);

    updateVertices(origin, size);

    int indices[] = {
            0, 1, 2,
            2, 1, 3
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, uvs);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);

    float uvN[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
    };
    float uvI[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    };

    float *uv = invertY ? uvI : uvN;

    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), uv, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


}


Plane::~Plane() {
}

void Plane::draw() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}

void Plane::updateVertices(vec2 origin, vec2 size) {
    this->origin = origin;
    this->size = size;
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

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
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Plane::Plane() : vao(-1), vbo(-1), ebo(-1), uvs(-1), origin(-1, -1), size(-1, -1), z(-1) {}

