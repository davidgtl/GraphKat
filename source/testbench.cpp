//
// Created by david on 28/12/2019.
//

#include "testbench.h"
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <random>
#include <chrono>
#include <dataflow/Context.h>
#include <utils/Randoms.h>
#include <utils/BitSet.h>
#include <typing/TypeInfos.h>
#include "typing/TypeInfo.h"

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

void TB::run_anyvector() {
    struct banana {
        int a;
        int b;
        int c;
    };
    using std::vector;
    using boost::any, boost::any_cast;

    vector<any> myvec;
    myvec.reserve(10);

    printf("\n first:  %p \n second: %p \n 10th:   %p \n", &myvec[0], &myvec[1], &myvec[2]);

    struct banana ban = {1, 2, 3};

    myvec[0] = (char) 5;
    myvec[1] = ban;
    myvec[2] = (double) 5.0;

    printf("\n first:  %p \n second: %p \n 10th:   %p \n", &myvec[0], &myvec[1], &myvec[2]);
    void *p = &myvec[1];
    printf("\n a: %d \n b: %p \n c: %p \n", any_cast<struct banana>(myvec[1]).c, &myvec[1], &myvec[2]);

}

void TB::deallocating() {
    using namespace std;
    const int size = 100000000;
    double *buffy = new double[size];

    for (int i = 0; i < size; i++)
        buffy[i] = i * 1.2;

    cout << "deallocate?\n";

    double sum = 0.0;
    for (int i = 0; i < size; i++)
        sum += buffy[i];
    cout << sum << endl;
    int a;
    cin >> a;

    void *p = buffy;

    delete[] p;
    cin >> a;
}

template<typename T, size_t offset>
T *access(void *pointy) {
    return (T *) ((char *) pointy + offset);
}

void TB::fancy_types() {
    void *mymem = new char[sizeof(int) + sizeof(double) + 10];
    auto myint = &access<int, 0>;
    auto mydouble = &access<double, sizeof(int)>;
    auto mystring = &access<char, sizeof(int) + sizeof(double)>;

    *myint(mymem) = 10;
    *mydouble(mymem) = 10.0 / 7;
    memset(mystring(mymem), 0, *myint(mymem));
    strcpy(mystring(mymem), "hello");

    cout << *myint(mymem) << " " << *mydouble(mymem) << " " << mystring(mymem);


}

namespace FancyTyping {
    template<typename... Types>
    size_t size_sum() {
        return (sizeof(Types)+...);
    }

    template<>
    size_t size_sum() {
        return 0;
    }

    template<typename T, typename... Types>
    T *access(void *mem) {
        return (T *) ((char *) mem + size_sum<Types...>());
    }

    template<void *mem, typename T, typename... Types>
    T *access() {
        return (T *) ((char *) mem + size_sum<Types...>());
    }


    template<typename... Types>
    void *allocate() {
        return new char[size_sum<Types...>()];
    }

    template<typename... Types>
    void copy(void *dest, void *src) {
        memcpy(dest, src, size_sum<Types...>());
    }
}

class FancyNode {
public:
    static constexpr auto create = &FancyTyping::allocate<int, double>;
    static constexpr auto copy = &FancyTyping::copy<int, double>;
    static constexpr auto age = &FancyTyping::access<int>;
    static constexpr auto kg = &FancyTyping::access<double, int>;

    void *mem;

    operator void *() {
        return this;
    }

    FancyNode() {
        mem = create();
    }

    FancyNode(void *mem) {
        this->mem = create();
        copy(this->mem, mem);
    }
};

void TB::fancy_access() {
    void *mem = FancyNode::create();
    *FancyNode::age(mem) = 11;
    *FancyNode::kg(mem) = 60.15 + *FancyNode::age(mem);

    cout << *FancyNode::age(mem) << " " << *FancyNode::kg(mem) << "\n";

    auto not_pointy = (FancyNode) mem;
    cout << *not_pointy.age(not_pointy) << " " << *not_pointy.kg(not_pointy) << "\n";
    cout << *not_pointy.age(not_pointy) << " " << *not_pointy.kg(not_pointy) << "\n";

    //FancyNode dupl1(not_pointy);
    FancyNode dupl2(mem);
/*
    //*dupl1.age() = 7;
    *dupl2.kg() = 30.2;

    //cout << *dupl1.age() << " " << *dupl1.kg() << "\n";
    cout << *dupl2.age() << " " << *dupl2.kg() << "\n";
    not_pointy._age(&not_pointy);
    */
}


class FancierNode {
public:

    void *mem;

    FancyTypes::TypeInfo *t;

    FancierNode(FancyTypes::TypeInfo &type) {
        this->t = &type;
        mem = t->create();
    }

    template<typename A>
    A access() {
        return (A) mem;
    }

    template<typename T, FancyTypes::t_iterator<T> f_iterator, FancyTypes::t_access<T> f_access>
    operator FancyTypes::TypeAccess<T, f_iterator, f_access>() {
        return (FancyTypes::TypeAccess<T, f_iterator, f_access>) mem;
    }

};

void TB::typeinfo() {
    using namespace FancyTypes;

    FancierNode node(int_i);

    int_t myint = node;

    myint = 5;

    cout << "myint: " << myint << "\n";

    auto myotherint = (int_t) node;

    cout << "myotherint: " << myotherint << "\n";
    myotherint = 7;
    cout << "myint: " << myint << "\n";


    /*
     * There need to be two behaviours:
     * 1. When using generics to get info in a standardized way -- print
     * 2. When accessing the variable in a function to get & set -- override = and T()
     */

}







