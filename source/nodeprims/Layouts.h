//
// Created by david on 21/12/2019.
//

#ifndef GRAPHKAT_LAYOUTS_H
#define GRAPHKAT_LAYOUTS_H

#include <ui/IMouseInteractable.h>
#include "utils/BitMap2D.h"
#include <vector>

namespace Layouts {
    void PopulateHitmap(BitMap2D<IMouseInteractable> &hitmap, const vector<IMouseInteractable *> &objects);
};


#endif //GRAPHKAT_LAYOUTS_H
