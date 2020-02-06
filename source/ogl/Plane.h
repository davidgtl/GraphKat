#pragma once

#include "PointModel.h"
#include <glm/glm.hpp>
#include <dataflow/Context.h>

using namespace glm;

class Plane {
public:
    unsigned int vao, vbo, ebo, uvs;

    Context *context;

    Plane();
    Plane(Context *ctx, bool invertY = false);

    ~Plane();

    void draw();

    void updateVertices();

    bool operator<(const Plane &obj) const {
        if (obj.vao < this->vao)
            return true;
        return false;
    }

    static void UpdateVertices(Context *in_ctx, Context *out_ctx);

    static Context *CreatePlane(vec2 origin, vec2 size, float z, Context *shader_ctx);

    static void RenderPlane(Context *in_ctx, Context *out_ctx);


private:

    void init(bool invertY = false);
};

