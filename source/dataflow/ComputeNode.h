//
// Created by david on 20/12/2019.
//

#ifndef GRAPHKAT_COMPUTENODE_H
#define GRAPHKAT_COMPUTENODE_H

#include <vector>

using std::vector;

class Context;
#define ComputeFunc(name) static void name(Context *in_ctx, Context *out_ctx)

class ComputeNode {
public:
    Context *inputs = nullptr;
    Context *outputs = nullptr;

    void (*func)(Context *in_ctx, Context *out_ctx);

    ComputeNode();

    void execute();
};


#endif //GRAPHKAT_COMPUTENODE_H
