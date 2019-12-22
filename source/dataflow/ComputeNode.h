//
// Created by david on 20/12/2019.
//

#ifndef GRAPHKAT_COMPUTENODE_H
#define GRAPHKAT_COMPUTENODE_H

#include <vector>
#include "nodeprims/macro_shenanigans.h"

using std::vector;

class Context;

typedef void (*ComputeFunc_t)(Context *, Context *);

#define ComputeFunc(name) static void name(Context *in_ctx, Context *out_ctx)
#define MAP_ENTRY(name, type) {typeid(type), CAT(_,name)<type>}
#define MAP_ENTRIES(name, ...) ApplyMacro(MAP_ENTRY, name, __VA_ARGS__)
#define ComputeFuncT(name, ...) private:\
template<typename T>\
static void CAT(_, name)(Context *in_ctx, Context *out_ctx);\
static std::unordered_map<std::type_index, ComputeFunc_t> *DCAT(_,name,_dynamic_map)() {\
    static std::unordered_map<std::type_index, ComputeFunc_t> smap = {\
            MAP_ENTRIES(name, __VA_ARGS__)\
    };\
    return &smap;\
}\
public:\
static void name(Context *in_ctx, Context *out_ctx) { (*DCAT(_,name,_dynamic_map)())[in_ctx->endpoint("x")->type()](in_ctx, out_ctx); }


class ComputeNode {
public:
    Context *inputs = nullptr;
    Context *outputs = nullptr;

    void (*func)(Context *in_ctx, Context *out_ctx);

    ComputeNode();

    void execute();
};


#endif //GRAPHKAT_COMPUTENODE_H
