//
// Created by david on 21/12/2019.
//

#include "Layouts.h"
#include <utils/BitMap2D.h>

void Layouts::PopulateHitmap(Context *in_ctx, Context *out_ctx) {

    auto objects = CIV(objects);
    auto hitmap = EIV(hitmap, BitMap2D<Context>*);

    for (auto o : objects->contexts()) {
        hitmap->insert(o.second);
        hitmap->set(o.second, EGV(o.second, primitive/origin, vec2), EGV(o.second, primitive/size, vec2));
    }

}
