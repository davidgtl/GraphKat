//
// Created by david on 27/03/2020.
//

#ifndef GRAPHKAT_ITERATOR_H
#define GRAPHKAT_ITERATOR_H

namespace FancyTypes {

    template<typename T>
    class iterator {
        void *index;

        bool (*next)();
    };

}


#endif //GRAPHKAT_ITERATOR_H
