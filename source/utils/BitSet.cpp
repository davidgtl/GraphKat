//
// Created by david on 29/12/2019.
//

#include "BitSet.h"

void BitSet::set(int i) {
    values[i] = true;
}

void BitSet::clear(int i) {
    values[i] = false;
}

vector<int> BitSet::intersect(const BitSet &bitset) {
    vector<int> res;

    for (int i = 0; i < size; i++)
        if (bitset.values[i] && values[i])
            res.push_back(i);

    return res;
}

BitSet::BitSet(int size) : size(size), values(size, false) {}

int BitSet::count() {
    int res = 0;
    for (int i = 0; i < size; i++)
        res += values[i] ? 1 : 0;
    return res;
}
