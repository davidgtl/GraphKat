//
// Created by David Ghitulescu on 2019-07-31.
//
#pragma once

#include <string>
#include "../ogl/Plane.h"
#include "wsize.h"
#include "LayoutConstraint.h"
#include <glm/glm.hpp>
#include <map>

using namespace glm;
using namespace std;

class LayoutManager {

public:
    vec2 screenSize;
    vec2 windowSize;

    LayoutManager(vec2 screenSize, vec2 windowSize);

    LayoutManager();

    ~LayoutManager() = default;

    void updateWindowSize(vec2 windowSize);

    void updateScreenSize(vec2 screenSize);

    void addPlane(const Plane &p, LayoutConstraint horizontal, LayoutConstraint vertical);

    void addPlane(Plane &p, Plane &parent, LayoutConstraint horizontal, LayoutConstraint vertical);

    tuple<vec2, vec2> calculateMetrics(Plane &p);

    vec2 sisc(vec2 size);

    vec2 sisc(float size);

    vec2 sisc(float sx, float sy);


private:
    map<Plane, Plane> parents;
    map<Plane, LayoutConstraint> hConstr;
    map<Plane, LayoutConstraint> vConstr;

    float resolveSize(wsize s, bool horizontal);

    tuple<float, float> resolveConstraint(LayoutConstraint con, bool horizontal);


};


