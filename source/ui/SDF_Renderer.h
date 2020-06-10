//
// Created by david on 10/06/2020.
//

#pragma once

#include "ui/IPlanePrimitive.h"
#include "ui/FMouseStateMgr.h"
#include "ui/Image.h"
#include "ogl/Texture.h"

class SDF_Renderer : public Image, private FMouseStateMgr {

private:
    Texture texture;
    ProgramShader *sdf_prog;
public:

    SDF_Renderer(vec2 origin, vec2 size, float z);

    void draw() override;
};



