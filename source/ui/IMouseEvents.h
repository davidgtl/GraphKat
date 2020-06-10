//
// Created by david on 10/06/2020.
//

#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class IMouseEvents {

public:
    enum Buttons : unsigned int {
        B1 = GLFW_MOUSE_BUTTON_1,
        B2 = GLFW_MOUSE_BUTTON_2,
        B3 = GLFW_MOUSE_BUTTON_3,
        B4 = GLFW_MOUSE_BUTTON_4,
        B5 = GLFW_MOUSE_BUTTON_5,
        B6 = GLFW_MOUSE_BUTTON_6,
        B7 = GLFW_MOUSE_BUTTON_7,
        B8 = GLFW_MOUSE_BUTTON_8,
        BLEFT = GLFW_MOUSE_BUTTON_LEFT,
        BRIGHT = GLFW_MOUSE_BUTTON_RIGHT,
        BMIDDLE = GLFW_MOUSE_BUTTON_MIDDLE
    };

    enum Actions {
        RELEASE = GLFW_RELEASE, PRESS = GLFW_PRESS
    };

    virtual void on_move(glm::vec2 pos) {}

    virtual void on_move_captured(glm::vec2 dpos) {}

    virtual void on_scroll(glm::vec2 off) {}

    virtual void on_leave() {}

    virtual void on_button(int button, int action, int mods) {}
};



