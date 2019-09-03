//
// Created by David Ghitulescu on 2019-08-27.
//

#pragma once

#include <tuple>
#include "wsize.h"
#include <iostream>

using namespace std;

class LayoutConstraint {
public:
    static LayoutConstraint sl(wsize start, wsize length);

    static LayoutConstraint se(wsize start, wsize end);

    static LayoutConstraint le(wsize length, wsize end);

    wsize start, length, end;
    enum types {
        slt, set, let
    } type;

    operator types() const { return type; }

    ~LayoutConstraint() = default;

    LayoutConstraint(float start, float end);

    LayoutConstraint();

    inline friend ostream &operator<<(ostream &os, const LayoutConstraint &obj) {
        os << '(' << obj.start << ',' << obj.length << ')';
        return os;
    }
};


