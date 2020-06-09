//
// Created by david on 09/06/2020.
//

#include "Slider.h"
#include "g_uniforms.h"

void Slider::draw() {
    shader->use();
    set_g_uniforms(*shader, size());
    shader->setUniform("color", color);
    shader->setUniform("value", value);
    IPlanePrimitive::draw();
}

Slider::Slider(vec3 color, float value, vec2 origin, vec2 size, float z)
        : IPlanePrimitive("slider", origin, size, z), color(color), value(value) {
}

void Slider::on_move(double x, double y) {
}

void Slider::on_scroll(double x, double y) {
}

void Slider::on_button(int button, int action, int mods) {
}
