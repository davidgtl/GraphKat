//
// Created by david on 09/06/2020.
//

#include "Button.h"
#include "g_uniforms.h"

void Button::draw() {
    shader->use();
    set_g_uniforms(*shader, size());
    shader->setUniform("color", color);
    shader->setUniform("value", value);
    IPlanePrimitive::draw();
}

Button::Button(vec3 color, float value, vec2 origin, vec2 size, float z)
        : IPlanePrimitive("button", origin, size, z), color(color), value(value) {
}

void Button::on_move(double x, double y) {
}

void Button::on_scroll(double x, double y) {
}

void Button::on_button(int button, int action, int mods) {
}