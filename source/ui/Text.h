//
// Created by david on 09/06/2020.
//

#pragma once

#include "ogl/Plane.h"
#include "ogl/ShaderLoader.h"
#include "ogl/ProgramShader.h"
#include "IMouseInteractable.h"
#include "ogl/IRenderable.h"
#include "ogl/FontRenderer.h"


class Text : public IMouseInteractable, IRenderable {
public:

    Text(FontRenderer &fr, string text, vec2 origin, vec2 size, float z,
         vec4 color, int align);

    void update_location(vec2 origin, vec2 size) {
        _origin = origin;
        _size = size;
    }

    vec2 origin() override {
        return _origin;
    }

    vec2 size() override {
        return _size;
    }

    void draw() override;

private:
    FontRenderer *fr;
    string text;
    vec4 color;
    int align;
    vec2 _origin;
    vec2 _size;
    float _z;

};



