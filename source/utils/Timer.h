//
// Created by david on 11/06/2020.
//

#pragma once

#include <chrono>

class timer {
private:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point stop;
public:
    timer() {
        stop = std::chrono::high_resolution_clock::now();
    }

    void tick() {
        start = stop;
        stop = std::chrono::high_resolution_clock::now();
    }

    float ms() {
        return std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1000.0f;
    }

};