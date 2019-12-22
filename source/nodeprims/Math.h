//
// Created by david on 22/12/2019.
//

#ifndef GRAPHKAT_MATH_H
#define GRAPHKAT_MATH_H

#include <dataflow/Context.h>
#include <dataflow/ComputeNode.h>

class Math {
private:
    template<typename T>
    static void _LinMap(Context *in_ctx, Context *out_ctx);

public:
    ComputeFunc(LinMap);

    ComputeFunc(LinMapUnit);

    ComputeFunc(InvLinMap);

    ComputeFunc(InvLinMapUnit);
};


#endif //GRAPHKAT_MATH_H
