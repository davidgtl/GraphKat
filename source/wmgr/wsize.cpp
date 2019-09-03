//
// Created by David Ghitulescu on 2019-08-22.
//

#include "wsize.h"

using namespace glm;

wsize::wsize(float value, units unit) : value(value), unit(unit) {}

wsize::wsize() : value(0), unit(none) {}

wsize::wsize(float value) : value(value), unit(perc) {}
