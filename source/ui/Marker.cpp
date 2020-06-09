//
// Created by david on 09/06/2020.
//

#include "Marker.h"
#include "g_uniforms.h"

Marker::Marker(vec3 color, int shape, int filled, vec2 border_size, vec2 origin, vec2 size, float z) :
        color(color), shape(shape), filled(filled), border_size(border_size),
        IPlanePrimitive("marker", origin, size, z) {
}

void Marker::draw() {
    shader->use();
    set_g_uniforms(*shader, size());
    shader->setUniform("color", color);
    shader->setUniform("shape", shape);
    shader->setUniform("filled", filled);
    shader->setUniform("border_size", border_size);
    IPlanePrimitive::draw();
}

void Marker::on_move(double x, double y) {
}

void Marker::on_scroll(double x, double y) {
}

void Marker::on_button(int button, int action, int mods) {
}
