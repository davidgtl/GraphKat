//
// Created by David Ghitulescu on 2019-07-31.
//

#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>
#include "LayoutManager.h"
#include "wsize.h"
#include <iostream>
#include <pugixml.hpp>
#include <fstream>

using namespace pugi;
using namespace std;

LayoutManager::LayoutManager() : screenSize(1, 1), windowSize(1, 1),
                                 hConstr(), vConstr(), parents() {

}

LayoutManager::LayoutManager(vec2 screenSize, vec2 windowSize) : screenSize(screenSize), windowSize(windowSize),
                                                                 hConstr(), vConstr(), parents() {

}

void LayoutManager::updateWindowSize(vec2 windowSize) {
    this->windowSize = windowSize;
}

void LayoutManager::updateScreenSize(vec2 screenSize) {
    this->screenSize = screenSize;
}

void LayoutManager::addPlane(const Plane &p, LayoutConstraint horizontal, LayoutConstraint vertical) {
    hConstr.insert({p, horizontal});
    vConstr.insert({p, vertical});

    for (auto const&[key, val] : vConstr) {
        std::cout << key.vao         // string (key)
                  << ':'
                  << val        // string's value
                  << std::endl;
    }
    cout << "----\n";
}

void LayoutManager::addPlane(Plane &p, Plane &parent, LayoutConstraint horizontal, LayoutConstraint vertical) {
    hConstr.insert({p, horizontal});
    vConstr.insert({p, vertical});
    parents.insert({p, parent});
}

tuple<vec2, vec2> LayoutManager::calculateMetrics(Plane &p) {

    auto[x, w] = resolveConstraint(this->hConstr[p], true);
    auto[y, h] = resolveConstraint(this->vConstr[p], false);

    vec2 origin = vec2(x, y);
    vec2 size = vec2(w, h);

    auto parentSize = vec2(1, 1);
    auto parentOrigin = vec2(0, 0);

    if (parents.count(p))
        tie(parentOrigin, parentSize) = calculateMetrics(parents[p]);

    origin += parentOrigin;
    size *= parentSize;

    p.updateVertices(origin, size);

    return {origin, size};
}

tuple<float, float> LayoutManager::resolveConstraint(LayoutConstraint con, bool horizontal) {

    if (con == LayoutConstraint::let) {
        auto size = resolveSize(con.length, horizontal);
        auto start = 1 - resolveSize(con.end, horizontal) - size;
        return {start, size};
    } else if (con == LayoutConstraint::slt) {
        auto start = resolveSize(con.start, horizontal);
        auto size = resolveSize(con.length, horizontal);
        return {start, size};
    } else {
        auto start = resolveSize(con.start, horizontal);
        auto size = 1 - resolveSize(con.end, horizontal) - start;
        return {start, size};
    }
}


float LayoutManager::resolveSize(wsize s, bool horizontal) {
    if (s == wsize::sis)
        return horizontal ? sisc(s).x : sisc(s).y;

    if (s == wsize::perc)
        return s;

    return 0;
}

vec2 LayoutManager::sisc(vec2 size) {
    return size * (screenSize.x / 1000.0f / windowSize);
}

vec2 LayoutManager::sisc(float size) {
    return vec2(size, size) * (screenSize.x / 1000.0f / windowSize);
}

vec2 LayoutManager::sisc(float sx, float sy) {
    return sisc(vec2(sx, sy));
}

LayoutManager::LayoutManager(const string layout_file) {
    pugi::xml_document doc;
    ifstream stream(layout_file);
    pugi::xml_parse_result result = doc.load(stream);

    pugi::xpath_node_set tools = doc.select_nodes("/Profile/Tools/Tool[@AllowRemote='true' and @DeriveCaptionFrom='lastparam']");

    std::cout << "Tools:\n";

    for (pugi::xpath_node_set::const_iterator it = tools.begin(); it != tools.end(); ++it)
    {
        pugi::xpath_node node = *it;
        std::cout << node.node().attribute("Filename").value() << "\n";
    }

    pugi::xpath_node build_tool = doc.select_node("//Tool[contains(Description, 'build system')]");

    if (build_tool)
        std::cout << "Build tool: " << build_tool.node().attribute("Filename").value() << "\n";
}
