//c
// Created by david on 11/06/2020.
//

#pragma once

#include <GLFW/glfw3.h>
#include <chrono>
#include <vector>
#include <list>

class Keyboard {
private:
    struct sample {
        std::chrono::system_clock::time_point time;
        bool down;
    };
    std::vector<std::list<sample>> history;
    GLFWwindow *window;
public:
    Keyboard(GLFWwindow *window);

    void callback(int key, int scancode, int action, int mods);

    bool is_down(uint32_t glfw_key);

    float pwm(uint32_t glfw_key, float ms);

};

inline Keyboard *win_keybd = nullptr;



