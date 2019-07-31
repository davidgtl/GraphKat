//
// Created by David Ghitulescu on 2019-07-31.
//
#pragma once

#include <string>
#include "../ogl/Plane.h"
#include <glm/glm.hpp>
#include <map>

using namespace glm;
using namespace std;

class LayoutManager {

public:
    LayoutManager(vec2 screenSize);

    ~LayoutManager() = default;

    void updateWindowSize(vec2 windowSize);

    void addPlane(string id, vec2 origin, vec2 size);

private:
    map<string, Plane> panels;
    map<string, vec2> orgins;
    map<string, vec2> sizes;
};


