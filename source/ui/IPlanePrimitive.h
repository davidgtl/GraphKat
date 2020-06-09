
//
// Created by david on 09/06/2020.
//

#pragma once

#include "ogl/Plane.h"
#include "ogl/ShaderLoader.h"
#include "ogl/ProgramShader.h"
#include "IMouseInteractable.h"
#include "ogl/IRenderable.h"

class IPlanePrimitive : public IMouseInteractable, public IRenderable {
public:
    Plane primitive;
    ProgramShader *shader;

    IPlanePrimitive(string shader_name, vec2 origin, vec2 size, float z)
            : primitive(origin, size, z), shader(&ShaderLoader::programMap[shader_name]) {

    }

    void draw() override {
        primitive.draw();
    }

    void update_location(vec2 origin, vec2 size) {
        primitive.updateVertices(origin, size);
    }

    vec2 origin() override {
        return primitive.origin();
    }

    vec2 size() override {
        return primitive.size();
    }
};