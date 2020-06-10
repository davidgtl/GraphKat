//
// Created by david on 10/06/2020.
//

#pragma once


#include "ui/IPlanePrimitive.h"
#include "ogl/Texture.h"

class Image : public IPlanePrimitive {
private:
    Texture *_texture;
public:
    Image(Texture *texture, vec2 origin, vec2 size, float z);

    void draw() override;

};



