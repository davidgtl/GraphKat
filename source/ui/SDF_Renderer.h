//
// Created by david on 10/06/2020.
//

#pragma once

#include "ui/IPlanePrimitive.h"
#include "ui/FMouseStateMgr.h"
#include "ui/Image.h"
#include "ui/IUpdateTick.h"
#include "ogl/Texture.h"

class SDF_Renderer : public Image, private FMouseStateMgr, public IUpdateTick {

private:
    vec3 eye_pos;
    float pitch, yaw;
    Texture texture;
    ProgramShader *sdf_prog;
    GLuint obj_data, obj_index;

    void compute_axis(vec3 &right, vec3 &forward, vec3 &up);

public:
    bool recompute = true;

    SDF_Renderer(vec2 origin, vec2 size, float z, void *index, int index_size, void *data, int data_size,
                 glm::vec3 bound_min, glm::vec3  bound_max, int levels);

    void draw() override;

    void on_move_captured(glm::vec2 dpos) override;

    void on_tick(float dt_ms) override;

    void update_z(float x);
};



