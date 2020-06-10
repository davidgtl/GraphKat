//
// Created by david on 10/06/2020.
//

#pragma once

#include "IMouseEvents.h"

class FMouseStateMgr : IMouseEvents {
public:
    void on_move(glm::vec2 pos) override {
        inside = true;
        this->pos = pos;
    }

    void on_leave() override {
        inside = false;
    }

    void on_button(int button, int action, int mods) override {
        if (action == GLFW_PRESS)
            b_down[button] = true;
        if (action == GLFW_RELEASE)
            b_down[button] = false;
    }

    bool is_inside() {
        return inside;
    }

    bool is_down(Buttons button) {
        return b_down[(unsigned int) button];
    }

    vec2 position() {
        return pos;
    }

private:
    vec2 pos;
    bool inside;
    bool b_down[8];
};