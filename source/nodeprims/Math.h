//
// Created by david on 22/12/2019.
//

#ifndef GRAPHKAT_MATH_H
#define GRAPHKAT_MATH_H

#include <dataflow/Context.h>
#include <dataflow/ComputeNode.h>

#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>

using glm::vec2;

class Math {

ComputeFuncT(LinMap, x);
ComputeFuncT(LinMapUnit, x);
ComputeFuncT(InvLinMap, x);
ComputeFuncT(InvLinMapUnit, x);

};


#endif //GRAPHKAT_MATH_H
