#include "Plane.h"
#include <glad/glad.h>
#include <nodeprims/Shaders.h>

Plane::Plane(bool invertY) : vao(-1), vbo(-1), ebo(-1), uvs(-1),
                             _origin(0, 0), _size(0, 0), _z(0), invertY(invertY) {
    init(invertY);
}

Plane::Plane(vec2 origin, vec2 size, float z, bool invertY) : vao(-1), vbo(-1), ebo(-1), uvs(-1),
                                                              _origin(origin), _size(size), _z(z), invertY(invertY) {
    init(invertY);
    updateVertices(origin, size, z);
}

void Plane::init(bool invertY) {

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &uvs);


    glBindVertexArray(vao);

    int indices[] = {
            0, 1, 2,
            2, 1, 3
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    float nothing[] = {0, 0, 0, 0,
                       0, 0, 0, 0,
                       0, 0, 0, 0};

    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), nothing, GL_STATIC_DRAW);

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

    float *uv = !invertY ? uvI : uvN;

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

void Plane::updateVertices(vec2 origin, vec2 size, float z) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    this->_origin = origin;
    this->_size = size;
    this->_z = z;

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

    glBufferSubData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), 0, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Plane::updateVertices(vec2 origin, vec2 size) {
    updateVertices(origin, size, this->_z);
}

vec2 &Plane::origin() {
    return _origin;
}

vec2 &Plane::size() {
    return _size;
}

float &Plane::z() {
    return _z;
}
