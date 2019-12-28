//
// Created by david on 29/12/2019.
//

#ifndef GRAPHKAT_BITSET_H
#define GRAPHKAT_BITSET_H

#include <vector>

using std::vector;

class BitSet {
private:
    vector<bool> values;
    int size;
public:
    explicit BitSet(int size);
    void set(int i);
    void clear(int i);
    vector<int> intersect(const BitSet &bitset);

    int count();

};


#endif //GRAPHKAT_BITSET_H
