//
// Created by David Ghitulescu on 2019-08-22.
//

#pragma once
#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>
#include <iostream>

using namespace glm;
using namespace std;

class wsize {
public:
    float value;
    enum units {
        sis, perc, none
    } unit;

    wsize(float value, units unit);

    wsize(float value);

    wsize();

    ~wsize() = default;

    operator float() const { return value; }

    operator units() const { return unit; }

    inline friend ostream &operator<<(ostream &os, const wsize &obj) {
        os << obj.value;
        if (obj.unit == sis)
            os << "s";
        if (obj.unit == perc)
            os << "p";
        return os;
    }
};


