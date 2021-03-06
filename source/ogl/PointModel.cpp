#include "PointModel.h"
#include "io.h"
#include <string>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include "messages.h"
#include <glm/glm.hpp>
#include <chrono>

using namespace std::chrono;
using namespace glm;

PointModel::PointModel() {
}

PointModel::PointModel(int width, int height) {
    vao = vbo_pos = vbo_cols = vbo_uvs = 0;
    this->width = width;
    this->height = height;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo_pos);
    glGenBuffers(1, &vbo_cols);
    glGenBuffers(1, &vbo_uvs);


    glBindVertexArray(vao);


    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_cols);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);

    /* COLORS */
    colors = new GLfloat[width * height * 3];

    updateSize(width, height);

}


PointModel::~PointModel() = default;


void PointModel::draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, width * height);
}

void PointModel::updateSize(int width, int height) {
    /* COLORS */
    this->width = width;
    this->height = height;

    delete[] colors;
    colors = new GLfloat[width * height * 3];

    /*VERTICES */
    auto *vertices_position = new GLfloat[width * height * 2];

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            vertices_position[y * width * 2 + x * 2 + 0] = 2.0f * x / width - 1.0;
            vertices_position[y * width * 2 + x * 2 + 1] = 2.0f * y / height - 1.0;
        }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, width * height * sizeof(float) * 2, vertices_position, GL_STATIC_DRAW);

    delete[] vertices_position;

    /* UVS */
    auto *uvs = new GLfloat[width * height * 2];

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            uvs[y * width * 2 + x * 2 + 0] = 2.0f * x / width - 1.0;
            uvs[y * width * 2 + x * 2 + 1] = 2.0f * y / height - 1.0;
        }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
    glBufferData(GL_ARRAY_BUFFER, width * height * sizeof(float) * 2, uvs, GL_STATIC_DRAW);

    delete[] uvs;

    update(0.0, 0.0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PointModel::update(float tx, float ty) {

    auto start = high_resolution_clock::now();

    const float len = 15;
    const float mod_freq = 0.5;
    const float mod_phase = 0.5;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            float dx = width / 2.0 - x + tx;
            float dy = height / 2.0 - y + ty;
            float r = sqrt(dx * dx + dy * dy);
            int index = (y * width + x) * 3;
            colors[index + 0] = (sin((r) / 3.14 / (len + mod_freq) - mod_phase) + 1.0) / 2;
            colors[index + 1] = (sin((r) / 3.14 / (len + 0) - mod_phase) + 1.0) / 2;
            colors[index + 2] = (sin((r) / 3.14 / (len - mod_freq)) + 1.0) / 2;
        }

    auto stop = high_resolution_clock::now();
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cols);
    glBufferData(GL_ARRAY_BUFFER, width * height * sizeof(float) * 3, colors, GL_STREAM_DRAW);

    auto duration = duration_cast<milliseconds>(stop - start);
    cout << duration.count() << endl;
}

