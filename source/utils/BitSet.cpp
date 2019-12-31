//
// Created by david on 29/12/2019.
//

#include <iostream>
#include "BitSet.h"

void BitSet::set(unsigned int i) {
    values[i >> LONG_POW] |= (cont_t) 1 << (i & LONG_MASK);
}

void BitSet::clear(unsigned int i) {
    values[i >> LONG_POW] &= ~((cont_t) 1 << (i & LONG_MASK));
}

vector<int> BitSet::intersect(const BitSet &bitset) {
    vector<int> res;

    for (unsigned int i = 0, absi = 0; i < size; i++, absi += LONG_SIZE) {
        cont_t intersection = bitset.values[i] & values[i];

        for (unsigned int o = 0; o < LONG_SIZE; o++, absi++)
            if (intersection & ((cont_t) 1 << o))
                res.push_back(absi);
    }

    return res;
}

BitSet::BitSet(unsigned int size) : size((size >> LONG_POW) + ((size & LONG_MASK) != 0)),
                                    values((size >> LONG_POW) + ((size & LONG_MASK) != 0), false) {}

int BitSet::count() {
    int res = 0;

    for (auto i : values) {
        for (unsigned int o = 0; o < LONG_SIZE; o++)
            if (i & ((cont_t) 1 << o))
                res++;
    }
    return res;
}

void BitSet::clear_regions(unsigned int a, unsigned int b) {

}

void BitSet::set_region(unsigned int a, unsigned int b) {

}

int BitSet::fintersect(const BitSet &bitset) {

    for (unsigned int i = 0, absi = 0; i < size; i++, absi += LONG_SIZE) {
        cont_t intersection = bitset.values[i] & values[i];

        for (unsigned int o = 0; o < LONG_SIZE; o++, absi++)
            if (intersection & ((cont_t) 1 << o))
                return absi;
    }

    return -1;
}
