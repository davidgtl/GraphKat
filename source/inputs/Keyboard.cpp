//
// Created by david on 11/06/2020.
//

#include "Keyboard.h"

float delta(std::chrono::system_clock::time_point end,
            std::chrono::system_clock::time_point start) {
    return std::chrono::duration_cast<std::chrono::microseconds>(
            end - start
    ).count() / 1000.0f;
}

void Keyboard::callback(int key, int scancode, int action, int mods) {
    auto now = std::chrono::high_resolution_clock::now();
    history[key].push_front({
            now,
            action == GLFW_PRESS
    });
    while (true) {
        if (history[key].empty())
            break;

        float duration = delta(now, history[key].back().time);
        if (duration > 1000)
            history[key].pop_back();
        else break;
    }
}

Keyboard::Keyboard(GLFWwindow *window) : window(window), history(GLFW_KEY_LAST + 1) {

}

bool Keyboard::is_down(uint32_t glfw_key) {
    return glfwGetKey(window, glfw_key) == GLFW_PRESS;
}

float Keyboard::pwm(uint32_t glfw_key, float ms) {
    auto now = std::chrono::high_resolution_clock::now();

    bool now_is_down = glfwGetKey(window, glfw_key) == GLFW_PRESS;
    auto last_time = now;
    float total = now_is_down ? ms : 0;

    bool parity = true;
    for (auto &sample : history[glfw_key]) {
        float dt = delta(now, sample.time);
        if (dt > ms)
            continue;

        if (sample.down == now_is_down)
            last_time = sample.time;
        else
            total += now_is_down ?
                     -delta(last_time, sample.time) :
                     +delta(last_time, sample.time);

        parity = !parity;
    }

    if (!parity)
        total += ms - (now_is_down ?
                       -delta(now, last_time) :
                       +delta(now, last_time));

    return total / ms;
}
