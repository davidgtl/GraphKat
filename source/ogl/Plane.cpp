#include "Plane.h"
#include <glad/glad.h>
#include <nodeprims/Shaders.h>

Plane::Plane(Context *ctx, bool invertY) : vao(-1), vbo(-1), ebo(-1), uvs(-1) {
    this->context = ctx;
    init(invertY);
}

void Plane::init(bool invertY) {

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &uvs);


    glBindVertexArray(vao);

    updateVertices();

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

void Plane::updateVertices() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    vec2 ostart = vec2(-1, -1);
    vec2 oend = vec2(1, 1);

    vec2 start = ostart + EGV(origin, vec2) * (oend - ostart);
    vec2 end = start + EGV(size, vec2) * (oend - ostart);

    auto z = EGV(z, float);

    float vertices[] = {
            start.x, start.y, z,
            end.x, start.y, z,
            start.x, end.y, z,
            end.x, end.y, z
    };

    glBufferData(GL_ARRAY_BUFFER, 4*3*sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Plane::Plane() : vao(-1), vbo(-1), ebo(-1), uvs(-1) {
    this->context = nullptr;
}

void Plane::UpdateVertices(Context *in_ctx, Context *out_ctx) {
    EIV(plane, Plane).updateVertices();
}

void Plane::RenderPlane(Context *in_ctx, Context *out_ctx) {
    Shaders::BindUniforms(in_ctx, nullptr);
    EGV(in_ctx, primitive/plane, Plane).draw();
}

Context *Plane::CreatePlane(vec2 origin, vec2 size, float z, Context *shader_ctx) {
    auto context = new Context();
    auto primitive = CCV(context, primitive);
    auto shader = CCV(context, shader);

    ECV(primitive, origin, origin);
    ECV(primitive, size, size);
    ECV(primitive, z, z);
    ECV(primitive, plane, Plane(primitive));

    auto cn_update_verts = new ComputeNode(primitive, nullptr, Plane::UpdateVertices);
    ERL(primitive, origin, cn_update_verts);
    ERL(primitive, size, cn_update_verts);
    ERL(primitive, z, cn_update_verts);

    shader->linkContext(shader_ctx);

    ECV(context, render, new ComputeNode(context, nullptr, Plane::RenderPlane));

    return context;
}
