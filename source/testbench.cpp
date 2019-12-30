//
// Created by david on 28/12/2019.
//

#include "testbench.h"
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <random>
#include <chrono>
#include <dataflow/Context.h>
#include <utils/Randoms.h>
#include <utils/BitSet.h>

using std::cout, std::unordered_map, std::vector, std::set;

int getFIndex(unsigned long a) {
    for (int i = 0; i < 64; i++) {
        if (a & 1) return i;
        a = a >> 1;
    }
    return -1;
}


int getCIndex(unsigned long a) {
    return (int) log2(a);
}

unordered_map<unsigned long, int> init_map() {
    unordered_map<unsigned long, int> res;
    unsigned long a = 1;
    for (int i = 0; i < 64; i++) {
        res[a] = i;
        a = a << 1;
    }
    return res;
}

int getLIndex(unsigned long a) {
    static auto lookup_table = init_map();
    return lookup_table[a];
}

void algo() {
    const int COMPONENTS = 10000000;
    const int CLICKS = 1000;
    const int W = 1920;
    const int H = 1080;

    vector<BitSet> ys;
    vector<BitSet> xs;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distx(0, W); // distribution in range [1, 6]
    std::uniform_int_distribution<std::mt19937::result_type> disty(0, H); // distribution in range [1, 6]
    std::uniform_int_distribution<std::mt19937::result_type> distw(10, 10); // distribution in range [1, 6]
    std::uniform_int_distribution<std::mt19937::result_type> disth(10, 10); // distribution in range [1, 6]

    cout << "Initializing sets\n";

    for (int ix = 0; ix <= W; ix++)
        xs.emplace_back(COMPONENTS);

    for (int iy = 0; iy <= H; iy++)
        ys.emplace_back(COMPONENTS);

    cout << "Populating sets\n";
    /*vector<Context *> contexts;
    for (int i = 0; i < COMPONENTS; i++) {
        contexts.push_back(new Context(Randoms::random_string()));
    }*/
    int area = 1;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < COMPONENTS; i++) {
        int x = distx(rng);
        int y = disty(rng);
        int w = distw(rng);
        int h = disth(rng);

        area += w + h;

        for (int ix = x; ix < x + w && ix < W; ix++)
            xs[ix].set(i);
        for (int iy = y; iy < y + h && iy < H; iy++)
            ys[iy].set(i);

    }

    auto finish = std::chrono::high_resolution_clock::now();

    std::cout << "Area: " << area << " Time/100px: "
              << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() / (1000.0f * area / 100)
              << " in "
              << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() / 1000.0f
              << " ms\n";

    int avg_fill = 0;
    for (int ix = 0; ix < W; ix++)
        avg_fill += xs[ix].count();
    for (int iy = 0; iy < H; iy++)
        avg_fill += ys[iy].count();

    cout << "avg_fill:" << avg_fill / (W + H) << "\n";

    vector<int> txs;
    vector<int> tys;

    for (int i = 0; i < CLICKS; i++) {
        txs.push_back(distx(rng));
        tys.push_back(disty(rng));
    }

    int intersections = 0;

    cout << "Intersecting sets\n";
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < CLICKS; i++) {
        intersections += xs[txs[i]].fintersect(ys[tys[i]]);
    }

    finish = std::chrono::high_resolution_clock::now();

    std::cout << "Intersections: " << intersections / (1.0f * CLICKS) << " in "
              << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() / (1000.0f * CLICKS)
              << " ms\n";
}

void test_indexing() {
    int SAMPLES;
    std::cin >> SAMPLES;
    vector<unsigned long> l;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 63); // distribution in range [1, 6]

    unsigned long one = 1;
    for (int i = 0; i < SAMPLES; i++)
        l.push_back(one << dist6(rng));

    // Record start time
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < SAMPLES; i++) {
        if (getFIndex(l[i]) < 0)
            cout << "negative\n";
    }

    auto finish = std::chrono::high_resolution_clock::now();

    std::cout << "For: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() / 1000.0f
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < SAMPLES; i++) {
        if (getCIndex(l[i]) < 0)
            cout << "negative\n";
    }

    finish = std::chrono::high_resolution_clock::now();

    std::cout << "Calculated: "
              << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() / 1000.0f << " ms\n";

    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < SAMPLES; i++) {
        if (getLIndex(l[i]) < 0)
            cout << "negative\n";
    }

    finish = std::chrono::high_resolution_clock::now();

    std::cout << "Lookup: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() / 1000.0f
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < SAMPLES; i++) {
        if (SAMPLES < 200) {
            if (getFIndex(l[i]) < 0)
                cout << "negative\n";
        } else {
            if (getCIndex(l[i]) < 0)
                cout << "negative\n";
        }
    }

    finish = std::chrono::high_resolution_clock::now();

    std::cout << "Best: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() / 1000.0f
              << " ms\n";

}

void TB::run_windowing() {
    algo();

}
