#pragma once

#include "PointModel.h"
#include <glm/glm.hpp>
#include "IRenderable.h"

using namespace glm;

class Plane : IRenderable {
public:
    unsigned int vao, vbo, ebo, uvs;

    explicit Plane(bool invertY = false);

    Plane(vec2 origin, vec2 size, float z, bool invertY = false);

    ~Plane();

    void draw() override;

    void updateVertices(vec2 origin, vec2 size, float z);

    void updateVertices(vec2 origin, vec2 size);

    vec2 &origin();

    vec2 &size();

    float &z();

    bool operator<(const Plane &obj) const {
        return obj.vao < this->vao;
    }

private:

    vec2 _origin;
    vec2 _size;
    float _z;
    bool invertY;

    void init(bool invertY = false);
};

