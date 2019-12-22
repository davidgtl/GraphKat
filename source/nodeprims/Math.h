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

private:
    template<typename T>
    static void _LinMap(Context *in_ctx, Context *out_ctx);
    static std::unordered_map<std::type_index, ComputeFunc_t> _LinMap_dynamic_map;
public:
    static void LinMap(Context *in_ctx, Context *out_ctx) {
        _LinMap_dynamic_map[in_ctx->endpoint("x")->type()](in_ctx, out_ctx);
    }

//    ComputeFuncT(LinMap);

    ComputeFunc(LinMapUnit);

    ComputeFunc(InvLinMap);

    ComputeFunc(InvLinMapUnit);
};


#endif //GRAPHKAT_MATH_H
