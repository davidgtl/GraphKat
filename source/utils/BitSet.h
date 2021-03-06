//
// Created by david on 29/12/2019.
//

#ifndef GRAPHKAT_BITSET_H
#define GRAPHKAT_BITSET_H

#include <vector>

using std::vector;

class BitSet {
private:
    typedef unsigned long long cont_t;
    vector<cont_t> values;
    unsigned int size;
#define LONG_SIZE 64
#define LONG_POW (unsigned int)6
#define LONG_MASK (unsigned int)63

public:
    explicit BitSet(unsigned int size);
    void set(unsigned int i);
    void set_region(unsigned int a, unsigned int b);
    void clear(unsigned int i);
    void clear_regions(unsigned int a, unsigned int b);
    vector<int> intersect(const BitSet &bitset);
    int fintersect(const BitSet &bitset);

    int count();

};


#endif //GRAPHKAT_BITSET_H
