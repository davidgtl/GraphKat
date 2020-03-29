//
// Created by david on 27/03/2020.
//

#ifndef GRAPHKAT_ITERATOR_H
#define GRAPHKAT_ITERATOR_H

#include <cstdlib>
#include <utility>
#include "TypeInfo.h"

namespace FancyTypes {

    template<std::size_t... Is>
    struct indices {
    };

    template<std::size_t N, std::size_t... Is>
    struct build_indices
            : build_indices<N - 1, N - 1, Is...> {
    };

    template<std::size_t... Is>
    struct build_indices<0, Is...> : indices<Is...> {
    };

    template<typename T, template<typename> typename C, typename ...Indexes>
    class iterator {

        using IL = std::make_index_sequence<sizeof...(Indexes)>;
        C<T> *container;
        size_t sizes[sizeof...(Indexes)];

    private:
        size_t mul(size_t index_i, size_t index_v) {
            return sizes[index_i] * index_v;
        }

    public:
        iterator(C<T> container, Indexes... sizes) : container(&container), sizes{sizes...} {}

        template<std::size_t... Is>
        size_t do_the_thing(std::index_sequence<Is...>, Indexes ...indexes) {
            return (mul(Is, indexes)+...);
        }

        T operator()(Indexes ...indexes) {
            return (*container)[do_the_thing(std::make_index_sequence<sizeof...(Indexes)>(), indexes...)];
        }
    };

}


#endif //GRAPHKAT_ITERATOR_H
