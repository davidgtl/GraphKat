//
// Created by david on 22/12/2019.
//

#include "Math.h"

#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>

using glm::vec2;

template<typename T>
void Math::_LinMap(Context *in_ctx, Context *out_ctx) {
    auto e = in_ctx->endpoint("fstart");
    auto a = EIV(fstart, T);
    auto b = EIV(fend, T);
    auto c = EIV(tstart, T);
    auto d = EIV(tend, T);

    EOV(res, T, (EIV(x, T) - a) / (b - a) * (d - c) + c);
}

typedef void (*fff)(Context *, Context *);

void Math::LinMap(Context *in_ctx, Context *out_ctx) {
    auto type = in_ctx->endpoint("x")->type();
    std::unordered_map<std::type_index, fff> dynamic_type = {
            {typeid(float), _LinMap<float>},
            {typeid(vec2),  _LinMap<vec2>}
    };

    dynamic_type[type](in_ctx, out_ctx);
}

void Math::LinMapUnit(Context *in_ctx, Context *out_ctx) {
    auto c = EIV(tstart, float);
    auto d = EIV(tend, float);
    EOV(fstart, float, EIV(x, float) * (d - c) + c);
}

void Math::InvLinMap(Context *in_ctx, Context *out_ctx) {
    auto a = EIV(fstart, float);
    auto b = EIV(fend, float);
    auto c = EIV(tstart, float);
    auto d = EIV(tend, float);

    EOV(fstart, float, (1 - (EIV(x, float) - a) / (b - a)) * (d - c) + c);
}

void Math::InvLinMapUnit(Context *in_ctx, Context *out_ctx) {
    auto c = EIV(tstart, float);
    auto d = EIV(tend, float);
    EOV(fstart, float, (1 - EIV(x, float)) * (d - c) + c);
}
