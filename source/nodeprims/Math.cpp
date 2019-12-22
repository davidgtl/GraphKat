//
// Created by david on 22/12/2019.
//

#include "Math.h"
#define GENERAL_ARITH_F float, vec2

LINK_TEMPLATE(Math, LinMap, GENERAL_ARITH_F)

template<typename T>
void Math::_LinMap(Context *in_ctx, Context *out_ctx) {
    auto a = EIV(fstart, T);
    auto b = EIV(fend, T);
    auto c = EIV(tstart, T);
    auto d = EIV(tend, T);

    EOV(res, T, (EIV(x, T) - a) / (b - a) * (d - c) + c);
}

LINK_TEMPLATE(Math, LinMapUnit, GENERAL_ARITH_F)

template<typename T>
void Math::_LinMapUnit(Context *in_ctx, Context *out_ctx) {
    auto c = EIV(tstart, T);
    auto d = EIV(tend, T);
    EOV(res, T, EIV(x, T) * (d - c) + c);
}

LINK_TEMPLATE(Math, InvLinMap, GENERAL_ARITH_F)

template<typename T>
void Math::_InvLinMap(Context *in_ctx, Context *out_ctx) {
    auto a = EIV(fstart, T);
    auto b = EIV(fend, T);
    auto c = EIV(tstart, T);
    auto d = EIV(tend, T);

    EOV(res, T, (1.0f - (EIV(x, T) - a) / (b - a)) * (d - c) + c);
}

LINK_TEMPLATE(Math, InvLinMapUnit, GENERAL_ARITH_F)

template<typename T>
void Math::_InvLinMapUnit(Context *in_ctx, Context *out_ctx) {
    auto c = EIV(tstart, T);
    auto d = EIV(tend, T);
    EOV(res, T, (1.0f - EIV(x, T)) * (d - c) + c);
}

