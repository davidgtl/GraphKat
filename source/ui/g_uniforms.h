//
// Created by david on 09/06/2020.
//

#pragma once

#include <glm/glm.hpp>
#include "ogl/ShaderLoader.h"
#include "nodeprims/Shaders.h"
#include "ogl/ProgramShader.h"
#include "wmgr/WindowLayout.h"

using namespace glm;


inline void set_g_uniforms(ProgramShader &shader, vec2 size) {

    vec2 g_uv_norm;
    float g_bw;
    float g_tr;

    win_layout.line_metrics(size, g_uv_norm, g_bw, g_tr);
    shader.setUniform("g_uv_norm", g_uv_norm);
    shader.setUniform("g_bw", g_bw);
    shader.setUniform("g_tr", g_tr);
}