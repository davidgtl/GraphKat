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

class WindowLayout {

public:
    vec2 screenSize;
    vec2 windowSize;
    vec2 windowNorm;

    WindowLayout(vec2 screenSize, vec2 windowSize);

    WindowLayout();

    WindowLayout(const string layout_file);

    ~WindowLayout() = default;

    void line_metrics(vec2 size, vec2 &g_uv_norm, float &g_bandwidth, float &g_transition);

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

    void recalculate_win_norm();

    tuple<float, float> resolveConstraint(LayoutConstraint con, bool horizontal);


};

inline WindowLayout win_layout;


