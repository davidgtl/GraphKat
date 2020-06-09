//
// Created by david on 09/06/2020.
//

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using glm::vec2;

class IMouseInteractable {
public:
    enum Buttons {
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

    virtual vec2 origin() = 0;

    virtual vec2 size() = 0;

    virtual void on_move(double x, double y) {};

    virtual void on_scroll(double x, double y) {};

    virtual void on_button(int button, int action, int mods) {};
};