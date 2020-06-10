//
// Created by david on 09/06/2020.
//

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "IMouseEvents.h"

using glm::vec2;

class IMouseInteractable : public IMouseEvents {
public:

    virtual vec2 origin() = 0;

    virtual vec2 size() = 0;
};