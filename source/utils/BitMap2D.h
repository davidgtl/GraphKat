//
// Created by david on 31/12/2019.
//

#ifndef GRAPHKAT_BITMAP2D_H
#define GRAPHKAT_BITMAP2D_H


#include "BitSet.h"
#include <list>
#include <unordered_map>
#include <boost/iterator/counting_iterator.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>

using std::cout;
using std::list, std::vector, std::unordered_map;
using glm::vec2;

template<typename T>
class BitMap2D {
private:
    int W, H, Comp;
    unordered_map<T *, int> objmap;
    unordered_map<int, T *> indexmap;
    list<int> available_indexes;
    vector<BitSet> ys;
    vector<BitSet> xs;

public:
    BitMap2D(int components, int width, int height) : xs(width, BitSet(components)),
                                                      ys(height, BitSet(components)),
                                                      available_indexes(boost::counting_iterator<int>(0),
                                                                        boost::counting_iterator<int>(components)),
                                                      objmap(), indexmap(), W(width), H(height), Comp(components) {
    }

    void set(T *obj, int x, int y) {
        int index = objmap[obj];
        ys[y].set(index);
        xs[x].set(index);
    }

    void set(T *obj, vec2 start, vec2 size) {
        int index = objmap[obj];

        vec2 end = start + size;
        vec2 mul = vec2(W, H);
        end = end*mul;
        start *= vec2(W, H);

        for (int x = start.x; x < (int) end.x; x++)
            xs[x].set(index);

        for (int y = start.y; y < (int) end.y; y++)
            ys[y].set(index);

    }

    void set(T *obj, vec2 nstart, vec2 nsize, vec2 ostart, vec2 osize) {
        int index = objmap[obj];

        vec2 nend = (nstart + nsize)*vec2(W, H);
        nstart *= vec2(W, H);

        vec2 oend = (ostart + osize)*vec2(W, H);
        ostart *= vec2(W, H);

        if (nstart.x < ostart.x)
            for (int x = nstart.x; x < (int) ostart.x; x++)
                xs[x].set(index);

        if (nstart.x > ostart.x) {
            int min = nstart.x < oend.x ? (int) nstart.x : (int) oend.x;
            for (int x = ostart.x; x < min; x++)
                xs[x].clear(index);
        }


        if (nstart.y < ostart.y)
            for (int y = nstart.y; y < (int) ostart.y; y++)
                ys[y].set(index);

        if (nstart.y > ostart.y) {
            int min = nstart.y < oend.y ? (int) nstart.y : (int) oend.y;
            for (int y = ostart.y; y < min; y++)
                ys[y].clear(index);
        }

    }

    void clear(T *obj, vec2 start, vec2 size) {
        int index = objmap[obj];

        vec2 end = (start + size)*vec2(W, H);
        start *= vec2(W, H);

        for (int x = start.x; x <= (int) end.x; x++)
            xs[x].clear(index);

        for (int y = start.x; y <= (int) end.y; y++)
            ys[y].clear(index);

    }

    void insert(T *val) {
        int index = available_indexes.back();
        available_indexes.pop_back();

        objmap[val] = index;
        indexmap[index] = val;
    }

    void remove(T *val) {
        int index = objmap[val];
        available_indexes.push_back(index);

        objmap.erase(val);
        indexmap.erase(index);
    }

    T *getFirst(int x, int y) {
        int index = ys[y].fintersect(xs[x]);
        return index == -1 ? nullptr : indexmap[index];
    }

    void prettyPrint() {
        //std::ofstream fout("hitmap.log", std::ios::out);
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                cout << ys[y].fintersect(xs[x]) << "|";
            }
            cout << "\n";
        }
    }
};


#endif //GRAPHKAT_BITMAP2D_H
