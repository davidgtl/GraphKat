//
// Created by david on 09/06/2020.
//

#pragma once

#include "ui/IPlanePrimitive.h"
#include "ui/FMouseStateMgr.h"

class Slider : public IPlanePrimitive, private FMouseStateMgr {
private:
    vec3 color;
    float value;

public:

    Slider(vec3 color, float value, vec2 origin, vec2 size, float z);

    void on_move(vec2 pos) override;

    void on_scroll(vec2 off) override;

    void on_leave() override;

public:
    void on_button(int button, int action, int mods) override;

    void draw() override;
};



