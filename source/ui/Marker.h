//
// Created by david on 09/06/2020.
//

#pragma once

#include "ui/IPlanePrimitive.h"


class Marker : public IPlanePrimitive {
private:
    vec3 color;
    int shape;
    int filled;
    vec2 border_size;

public:

    Marker(vec3 color, int shape, int filled, vec2 border_size, vec2 origin, vec2 size, float z);

    void on_move(vec2 pos) override;

    void on_scroll(vec2 off) override;

    void on_button(int button, int action, int mods) override;

    void draw() override;
};



