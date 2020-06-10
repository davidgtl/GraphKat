//
// Created by david on 21/12/2019.
//

#include "Layouts.h"
#include <utils/BitMap2D.h>
#include <ui/IMouseInteractable.h>

namespace Layouts {
    void PopulateHitmap(BitMap2D<IMouseInteractable> &hitmap, const vector<IMouseInteractable *> &objects) {
        for (IMouseInteractable *o : objects) {
            hitmap.insert(o);
            hitmap.set(o, o->origin(), o->size());
        }
    }

    void PopulateHitmap(BitMap2D<IMouseInteractable> &hitmap, IMouseInteractable *object) {
        hitmap.insert(object);
        hitmap.set(object, object->origin(), object->size());
    }
}
