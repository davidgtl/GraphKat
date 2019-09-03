//
// Created by David Ghitulescu on 2019-08-27.
//

#include "LayoutConstraint.h"

LayoutConstraint::LayoutConstraint() : start(), length(), end(), type(set) {}

LayoutConstraint::LayoutConstraint(float start, float end) : start(start), length(1 - end - start), end(end),
                                                             type(set) {
}

LayoutConstraint LayoutConstraint::sl(wsize start, wsize length) {
    auto res = LayoutConstraint();

    res.start = start;
    res.length = length;
    res.type = slt;

    return res;
}

LayoutConstraint LayoutConstraint::se(wsize start, wsize end) {
    auto res = LayoutConstraint();

    res.start = start;
    res.end = end;
    res.type = set;

    return res;
}

LayoutConstraint LayoutConstraint::le(wsize length, wsize end) {
    auto res = LayoutConstraint();

    res.length = length;
    res.end = end;
    res.type = let;

    return res;
}

