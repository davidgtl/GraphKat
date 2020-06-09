//
// Created by david on 09/06/2020.
//

#pragma once

#include "ui/IPlanePrimitive.h"

class Slider : public IPlanePrimitive {
private:
    vec3 color;
    float value;

public:

    Slider(vec3 color, float value, vec2 origin, vec2 size, float z);

    void on_move(double x, double y) override;

    void on_scroll(double x, double y) override;

    void on_button(int button, int action, int mods) override;

    void draw() override;
};



