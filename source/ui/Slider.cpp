//
// Created by david on 09/06/2020.
//

#include "Slider.h"
#include "g_uniforms.h"

void Slider::draw() {
    shader->use();
    set_g_uniforms(*shader, size());
    auto cColor = color * (is_inside() ? 1.0f : 0.7f);
    shader->setUniform("color", cColor);
    shader->setUniform("value", value);
    IPlanePrimitive::draw();
}

Slider::Slider(vec3 color, float value, vec2 origin, vec2 size, float z)
        : IPlanePrimitive("slider", origin, size, z), color(color), value(value) {
}

void Slider::on_move(vec2 pos) {
    FMouseStateMgr::on_move(pos);
    if (FMouseStateMgr::is_down(Buttons::BLEFT))
        value = ((pos - origin()) / size()).x;
}

void Slider::on_scroll(vec2 off) {
}

void Slider::on_button(int button, int action, int mods) {
    FMouseStateMgr::on_button(button, action, mods);
    if (FMouseStateMgr::is_down(Buttons::BLEFT))
        value = ((FMouseStateMgr::position() - origin()) / size()).x;
}

void Slider::on_leave() {
    FMouseStateMgr::on_leave();
}
