//
// Created by david on 28/12/2019.
//

#include "testbench.h"
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>
#include <list>
#include <set>
#include <random>
#include <chrono>
#include <utils/Randoms.h>
#include <utils/BitSet.h>
#include <typing/TypeInfos.h>
#include <typing/iterator.h>
#include "typing/TypeInfo.h"
#include <boost/format.hpp>
#include <boost/any.hpp>

using std::cout, std::unordered_map, std::vector, std::set, std::list, std::string;

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
        t->initialize(mem);
    }

    template<typename A>
    A access() {
        return (A) mem;
    }

    /*template<typename T, FancyTypes::t_iterator<T> f_iterator, FancyTypes::t_access<T> f_access, FancyTypes::TypeInfo &typeinfo>
    operator FancyTypes::TypeAccess<T, f_iterator, f_access, typeinfo>() {
        return (FancyTypes::TypeAccess<T, f_iterator, f_access, typeinfo>) mem;
    }*/

};

void TB::typeinfo() {
    using namespace FancyTypes;

#if 0
    FancierNode node(int_i);

    int_t myint = node;

    myint = 5;

    cout << "myint: " << myint << "\n";

    auto myotherint = (int_t) node;

    cout << "myotherint: " << myotherint << "\n";
    myotherint = 7;
    cout << "myint: " << myint << "\n";
#endif

    FancierNode node(uninfo_i);

    string_t st = string("hello");

    cout << st << "\n";

    uninfo_t info = node.mem;

    info = {"Bernadette", 4, 1, 3};
    uninfo_t info2 = {"Bernadette", 2, 1, 3};

    uninfo_t info3 = *info + *info2;

    vector<float> myfile(1000);

    for (int i = 0; i < 1000; i++)
        myfile[i] = i / 3.0f;

    for (int y = 0; y < 10; y++)
        for (int x = 0; x < 10; x++)
            cout << myfile[y * 10 + x] << " ";

    cout << "\n";
    iterator dim2(myfile, 10, 10, 3);

    for (int y = 0; y < 10; y++)
        for (int x = 0; x < 10; x++)
            cout << dim2(1, y, x) << " ";


    /*auto l = I(info).length;

    cout << info->length << " = " << l << "\n";*/
    cout << info.toString() << "\n";
    cout << info3.toString() << "\n";

    cout << "glm tests \n";

    vec3_t a({1.0, 0.5, 0.5});
    vec3_t b({1.0, 0.5, 1.33});

    vec3_t c = *a + *b;

    cout << c;

    cout << "\nDone.";
}

// Oh the lag..
/*
#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL(...)  EVAL4(EVAL4(EVAL4(__VA_ARGS__)))

#define MAP_END(...)
#define MAP_OUT
#define MAP_COMMA ,

#define MAP_GET_END2() 0, MAP_END
#define MAP_GET_END1(...) MAP_GET_END2
#define MAP_GET_END(...) MAP_GET_END1
#define MAP_NEXT0(test, next, ...) next MAP_OUT
#define MAP_NEXT1(test, next) MAP_NEXT0(test, next, 0)

#define MAP_NEXT(test, next)  MAP_NEXT1(MAP_GET_END test, next)

#define MAP0(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP1)(f, peek, __VA_ARGS__)
#define MAP1(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP0)(f, peek, __VA_ARGS__)

#define MAP_LIST_NEXT1(test, next) MAP_NEXT0(test, MAP_COMMA next, 0)
#define MAP_LIST_NEXT(test, next)  MAP_LIST_NEXT1(MAP_GET_END test, next)

#define MAP_LIST0(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST1)(f, peek, __VA_ARGS__)
#define MAP_LIST1(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST0)(f, peek, __VA_ARGS__)

#define MAP_2P0(f, x, y, peek, ...) f(x, y) MAP_NEXT(peek, MAP_2P1)(f, peek, __VA_ARGS__)
#define MAP_2P1(f, x, y, peek, ...) f(x, y) MAP_NEXT(peek, MAP_2P0)(f, peek, __VA_ARGS__)

#define MAP_2P_LIST0(f, x, y, peek, ...) f(x, y) MAP_LIST_NEXT(peek, MAP_2P_LIST1)(f, peek, __VA_ARGS__)
#define MAP_2P_LIST1(f, x, y, peek, ...) f(x, y) MAP_LIST_NEXT(peek, MAP_2P_LIST0)(f, peek, __VA_ARGS__)


#define MAP_2P(f, ...) EVAL(MAP_2P1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))


#define MAP_2P_LIST(f, ...)  EVAL(MAP_2P_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))


#define MAP(f, ...) EVAL(MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))


#define MAP_LIST(f, ...) EVAL(MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define STRINGIFY(x) #x
#define CALL(x, y) printf(STRINGIFY(x))
*/

void TB::macro_parenthesis() {
    //MAP_2P_LIST(CALL, a, b, c, d);
}

template<typename T, template<typename...> typename Template>
struct is_specialization : std::false_type {
};

template<template<typename...> typename Template, typename... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {
};

//template <typename T, template <typename...> typename Template>
//using is_specialization_v = typename is_specialization<T, Template>::value;


template<int N, typename... Args>
struct nth_arg;

template<typename Head, typename... Rest>
struct nth_arg<0, Head, Rest...> {
    typedef Head type;
};

template<int N, typename Head, typename... Rest>
struct nth_arg<N, Head, Rest...> {
    typedef typename nth_arg<N - 1, Rest...>::type type;
};

template<typename S, template<typename...> typename T>
struct argument_extractor;

template<template<typename...> typename C, typename... Args>
struct argument_extractor<C<Args...>, C> {
    template<int N>
    using nth_arg = typename nth_arg<N, Args...>::type;
};


/*
inline typename std::enable_if<(not std::is_same<T, int>::value)
                               and (not std::is_same<T, float>::value), void>::type
*/
/*
template <template <typename> typename C, typename T>
constexpr inline typename std::enable_if<std::is_pointer<C<T>>::value, int>::type
gimme_info(){
    auto content = gimme_info<std::remove_pointer_t<C<T>>>();
    return 200 + content;
}

template <template <typename> typename C, typename T>
constexpr inline typename std::enable_if<not std::is_pointer<C<T>>::value, int>::type
gimme_info(){}
*/

template<typename, typename Condition=void>
struct gimme_info;

template<>
struct gimme_info<int> {
    static constexpr int value = 1;
};

template<>
struct gimme_info<float> {
    static constexpr int value = 2;
};

template<typename A, typename B, typename C>
struct mycomplexthing {
};

//std::enable_if_t<std::is_pointer<T>::value, int> = 0
template<typename T>
struct gimme_info<T, typename std::enable_if_t<std::is_pointer_v<T>>> {
    static constexpr int value = 10 * gimme_info<std::remove_pointer_t<T>>::value;
};

template<typename C>
struct gimme_info<C, typename std::enable_if_t<is_specialization<C, std::vector>::value>> {
    static constexpr int value =
            5 + 10 * gimme_info<typename argument_extractor<C, std::vector>::template nth_arg<0>>::value;
};

template<typename C>
struct gimme_info<C, typename std::enable_if_t<is_specialization<C, mycomplexthing>::value>> {
    static constexpr int value =
            (((gimme_info<typename argument_extractor<C, mycomplexthing>::template nth_arg<0>>::value) * 10
              + gimme_info<typename argument_extractor<C, mycomplexthing>::template nth_arg<1>>::value) * 10
             + gimme_info<typename argument_extractor<C, mycomplexthing>::template nth_arg<2>>::value) * 10 + 6;
};

/*
// Value of <T>
template <typename T>
constexpr inline typename std::enable_if<not std::is_pointer<T>::value, int>::type
gimme_info();

template <>
constexpr int gimme_info<int>(){
    return 1;
}

template <>
constexpr int gimme_info<float>(){
    return 2;
}

// Value of <T*>
template <typename T>
constexpr inline typename std::enable_if<std::is_pointer<T>::value, int>::type
gimme_info(){
    auto content = gimme_info<std::remove_pointer_t<T>>();
    return 10 + content;
}
*/
// Value of <C<T>>
//template <typename C, typename T>
//constexpr inline typename std::enable_if<not std::is_pointer<C<T>>::value, int>::type
//constexpr inline int
//gimme_info();

/*
template <template <typename...> typename C, typename T>
constexpr inline typename std::enable_if<(not std::is_pointer<C<T>>::value) and std::is_same<C<T>, vector<T>>::value, int>::type
gimme_info(){
    //auto content = gimme_info<T>();
    return 100;// + content;
}
*/
/*
template <typename C>
constexpr inline typename std::enable_if_t<is_specialization<C, std::vector>::value, int>
gimme_info(){
    auto content = gimme_info<argument_extractor<C, std::vector>::template nth_arg<1>>();
    return 100 + content;
}
*/
/*
template<typename T>
struct banana{
    template <typename C>
    banana(){

    }
}
 */


//template<typename C>
//using extract_args = argument_extractor<C>;



/*
template <template <typename> typename C, typename T>
constexpr inline typename std::enable_if<(not std::is_pointer<C<T>>::value) and std::is_same<C<T>, vector<T>>::value, int>::type
gimme_info(){
    auto content = gimme_info<T>();
    return 10 + content;
}
*/


void TB::templating() {
    using boost::format;
    cout << format("1 = %1%\n") % gimme_info<int>::value;
    cout << format("2 = %1%\n") % gimme_info<float>::value;
    cout << format("10 = %1%\n") % gimme_info<int *>::value;
    cout << format("10 = %1%\n") % gimme_info<float *>::value;
    //cout << format("err = %1%\n") % gimme_info<double>::value;
    //cout << format("err = %1%\n") % gimme_info<double*>::value;
    cout << format("15 = %1%\n") % gimme_info<vector<int>>::value;
    cout << format("25 = %1%\n") % gimme_info<vector<float>>::value;
    cout << format("105 = %1%\n") % gimme_info<vector<int *>>::value;
    cout << format("205 = %1%\n") % gimme_info<vector<float *>>::value;
    cout << format("150 = %1%\n") % gimme_info<vector<int> *>::value;
    cout << format("250 = %1%\n") % gimme_info<vector<float> *>::value;
    cout << format("1050 = %1%\n") % gimme_info<vector<int *> *>::value;
    cout << format("2050 = %1%\n") % gimme_info<vector<float *> *>::value;
    cout << format("115265 = %1%\n") % gimme_info<vector<mycomplexthing<int, vector<int>, float>>>::value;
}


void TB::birthday() {
    using namespace std;
    using boost::format;

    long double k = 3.141592653589793238462643383279502884L;

    long double best_error = k;
    uint64_t start = 7;
    for (uint64_t i = 1; i < 1000000; i++) {
        long double cn = k * i;
        long double err = 0.5L - abs(cn - floor(cn) - 0.5L);
        if (err < best_error) {
            best_error = err;
            cout << i << " " << err << "\n";
            cout << (i - 2) % 113 << "\n";
        }
    }
    glm::vec3(0, 0, 0);
}

void fun(int *&myptr) {
    cout << "Hello from &\n";
}

void fun(int *&&myptr) {
    cout << "Hello from &&\n";
}

void TB::movingpointers() {
    int *p = new int(5);

    fun(p);
    fun(std::move(p));

    fun(new int(5));
    fun(std::move(new int(5)));

    //I love successful tests
}

#define _A(k, a, b) k::a + k::b
#define _B(k, a, b) k::a * k::b

#define MCAT(a, b, c) a ## b ## C
#define A(a, b) MCAT(k, ::, a) + MCAT(k, ::, b)

#define C(k, m) m

struct mystruct_macro_order {
    constexpr static int k1 = 1;
    constexpr static int k2 = 2;
};

void TB::mystruct_macro_order() {
    //cout<<C(mystruct, C(mystruct, A(k1,k2)));
}

struct structuring_vector_struct {
    int a;
    int b;
    int c;
    int d;
    int e;
};

void TB::structuring_vector() {
    vector<int> v = {1, 2, 3, 4, 5};

    auto &s = *(structuring_vector_struct *) v.data();

    cout << s.a << s.b << s.c << s.d << s.e;
}

void isit(int *&lvalue) {
    cout << "nope\n";
}

void isit(int *&&rvalue) {
    cout << "yep\n";
}

template<typename T>
void isit_ref(T &&lr_ref) {
    // static_assert(std::is_same_v<std::remove_reference_t<T>, int *>,
    //         "This function does not support the specified type.");
    isit(std::forward<T>(lr_ref));
}

void TB::isRValue() {
    int *k = new int(3);
    isit(new int(5));
    isit(k);
    isit_ref(new int(10));
    isit_ref(k);
    //isit_ref(5);
}

struct scope_node {
    vector<void *> inputs;
    vector<void *> outputs;
};

struct scope_escape {

    typedef scope_escape this_t;
    scope_node *value;

    void link(size_t *what, scope_node *to, size_t where) {

    }

    enum class Inputs : size_t {
        a = 2, b, c
    };

    typedef struct {
        typedef size_t a;
        typedef int b;
        typedef double c;
    } Inputs_t;


    enum class Outputs : size_t {
        a = 2, b, c
    };

    typedef struct {
        typedef size_t a;
        typedef int b;
        typedef double c;
    } Outputs_t;


};

#define link_scope(dest, dest_where, src, src_where) \
dest.value->inputs[(size_t) decltype(dest)::Inputs::dest_where] = \
src.value->outputs[(size_t) decltype(src)::Outputs::src_where]


void TB::scope_escape_tb() {
    typedef int myint;
    scope_escape::this_t::this_t::Inputs_t::a a = (size_t) scope_escape::Inputs::a;
    cout << (myint) a;

    scope_escape n1, n2;
    typedef decltype(n1)::Inputs n1_ii;
    typedef decltype(n1)::Outputs n1_oi;
    typedef decltype(n2)::Inputs n2_ii;
    typedef decltype(n2)::Outputs n2_oi;

    auto x = new scope_escape::Outputs_t::b();

    //n1.value->inputs[n1.Inputs.a] = n2.value->outputs[n2.Outputs.b];
    //n1.value->inputs[(size_t) decltype(n1)::input_indexes::a] =
    //        n2.value->outputs[(size_t) decltype(n2)::input_indexes::b];

    link_scope(n1, a, n2, b);

}

struct SynSugar {
    int a;
    enum class idx : std::size_t {
        a, b, c, __end
    };
    int r[(size_t) idx::__end] = {1, 2, 3};

    SynSugar(int x) {
        a = x;
    }

    void operator()() {
        cout << "Hello world! " << (size_t) idx::a;
    }
};

void TB::pointy_parant() {
    SynSugar *a = new SynSugar(1);

    (*a)();
    SynSugar &b = *a;

    b();
}

template<size_t index, size_t... values>
struct ct_lut;

template<size_t cur, size_t... values>
struct ct_lut<0, cur, values...> {
    static constexpr size_t value = cur;
};

template<size_t index, size_t cur, size_t... values>
struct ct_lut<index, cur, values...> {
    static constexpr size_t value = ct_lut<index - 1, values...>::value;
};

struct abstract_banana {
    virtual void do_the_flop() = 0;
};

struct banana2 : abstract_banana {

    int k = 0;
    template<size_t N>
    static constexpr size_t lut = ct_lut<N, 1, 2, 3, 4, 5>::value;

    static constexpr size_t arr[5] = {1, 2, 3, 4, 5};
    static const size_t *arr2[3];

    static void print_bananas(bool &var) {
        cout << arr[0];
        var = false;
    }

    void do_the_flop() {
        cout << "|_";
    }

    banana2 &do_the_self_flop() {
        cout << "k: " << k << "\n";
        k++;
        return *this;
    }

    typedef struct {
        typedef float z;
        typedef int start;
        typedef int end;
    } Inputs_t;
};

typedef banana2 banana2;

void TB::lut() {
    bool IamTRUE = true;
    banana2::print_bananas(IamTRUE);
    cout << "I am false: " << IamTRUE;
    abstract_banana **arr = new abstract_banana *[2];
    arr[0] = new banana2();
    arr[0]->do_the_flop();

    banana2().do_the_self_flop().do_the_self_flop().do_the_self_flop();

    banana2::Inputs_t::end a = 2;
}

typedef void (*bad_lambda_func)(vector<int *> &inputs, vector<int *> &outputs, bool &return_control_flow);

bad_lambda_func gimme_good_func() {
    return [](vector<int *> &inputs, vector<int *> &outputs, bool &return_control_flow) {
        static int data = 5;
        cout << "Hello func!\n";
        cout << data++ << "\n";
        return_control_flow = true;
    };
}

void TB::bad_lambda() {
    bad_lambda_func func1 = gimme_good_func();
    bad_lambda_func func2 = gimme_good_func();

    bool ok = false;
    vector<int *> ins = {new int(2)};
    vector<int *> outs = {new int(3)};
    func1(ins, outs, ok);
    func2(ins, outs, ok);
    func1(ins, outs, ok);
    func2(ins, outs, ok);
    cout << "ok: " << ok << "\n";
};

template<typename T>
struct pika {
    T data;

    pika(const T &val) : data(val) {
        cout << "is it l? " << std::is_lvalue_reference_v<T> << "\n";
        cout << "is it r? " << std::is_rvalue_reference_v<T> << "\n";
//        cout << "ref\n";
        if (std::is_same_v<T, int>) {
            cout << "spec\n";
        } else {
            cout << "tmp\n";
        }
    }

    pika(const int *val) : data(val) {
        cout << "int*ss\n";
    }

    operator int *() {
        return new int(5);
    }

    int &gimme_data() &{
        return *new int(7);
    }

    int &&gimme_data() &&{
        return std::move(*new int(7));
    }
};

template<typename T>
void lr_info(T &data) {
    cout << "data is lvalue\n";
}

template<typename T>
void lr_info(T &&data) {
    cout << "data is rvalue\n";
}

void TB::pikapika() {
    auto grr = pika(string("a"));
    string x = "Hellooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo\n";
    auto grrr = pika(x);
    cout << x;
    auto nyaa = pika(5);
    auto nya = pika(new int(5));

    cout << *(int *) nya;


    lr_info(x);
    lr_info(new int(5));
    cout << "member tests:\n";
    lr_info(nya.gimme_data());
    lr_info(pika(new int(5)).gimme_data());
}

void dosth_opt(std::optional<bool *> dosth = {}) {
    if (dosth.has_value()) {
        if (*dosth != nullptr && **dosth == true)
            cout << "I received: true\n";
        else if (*dosth != nullptr && **dosth == false)
            cout << "I received: false\n";
        else
            cout << "What is this fakery\n";
    } else {
        cout << "Nothing mate \n";
    }
}

void TB::optional() {
    dosth_opt();
    dosth_opt(new bool(true));
    dosth_opt(new bool(false));
    dosth_opt(nullptr);
}

int cr_func(int &x) {
    return x + 5;
}

constexpr int cr_func(const int &x) {
    return x + 1;
}

void TB::cr_test() {
    cout << cr_func(5);
    int k = time(nullptr) % 3;
    cout << cr_func(k);
}


void TB::vector_init() {
    vector<int *> myvec = vector<int *>(5);

    myvec.push_back(new int(3));
    cout << "inserted at: " << myvec.size() - 1 << "\n";
    for (int i = 0; i < myvec.size(); i++) {
        if (myvec[i] != nullptr)
            cout << "[" << i << "]: " << *myvec[i] << "\n";
    }

    vector<int> z;
    //R.I.P. last ounce of consistency
    z.push_back(0);
    //z.push_back({1,2,3});
    z.insert(z.end(), 4);
    z.insert(z.end(), {5, 6, 7});
    z = {1, 2, 3};
    for (auto x : z) {
        cout << x << " ";
    }
}

template<typename T, typename Cond = void>
struct is_const_char_array : std::false_type {
};


template<typename T>
struct is_const_char_array<T,
        std::enable_if_t<std::is_array_v<std::remove_reference_t<T>> and
                         std::is_same_v<const char, std::remove_extent_t<std::remove_reference_t<T>>>
        >> : std::true_type {
};

template<typename T>
inline constexpr bool is_const_char_array_v = is_const_char_array<T>::value;

template<typename T, int N = 3>
struct fwd_tag {
};

template<typename T, int N = 0>
struct forwarder {
    typedef T type;
    static const int n = N;
};

template<typename T, int N>
struct forwarder<fwd_tag<T, N>> {
    typedef T type;
    static const int n = N;
};

template<typename T>
using forwarder_t = typename forwarder<T>::type;
template<typename T>
inline const int forwarder_n = forwarder<T>::n;


void TB::is_T() {
    const char (&var)[9] = "position";
    cout << std::is_array_v<std::remove_reference_t<decltype(var)>>;
    cout << std::is_lvalue_reference_v<decltype(var)>;
    cout << std::is_same_v<const char, std::remove_extent_t<std::remove_reference_t<decltype(var)>>>;
    cout << "\n...\n";
    cout << is_const_char_array_v<const char (&)[9]>;
    cout << is_const_char_array_v<const char>;
    cout << is_const_char_array_v<char *>;
    cout << is_const_char_array_v<char *&&>;
    cout << is_const_char_array_v<const char *>;
    cout << is_const_char_array_v<const char *[10]>;
    cout << is_const_char_array_v<const char[10]>;
    cout << is_const_char_array_v<const char[]>;
    cout << is_const_char_array_v<decltype("ana")>;
    cout << "\n...\n";
    cout << is_const_char_array_v<forwarder_t<const char (&)[9]>>;
    cout << is_const_char_array_v<forwarder_t<fwd_tag<const char (&)[9]>>>;
    cout << "\n n: " << forwarder_n<fwd_tag<const char (&)[9], 7>> << "\n";
    cout << "\n n: " << forwarder_n<fwd_tag<const char (&)[9]>> << "\n";
    cout << "\n n: " << forwarder_n<const char (&)[9]> << "\n";

    int data = 5;
    int *dataptr = &data;

    lr_info(&data);
    lr_info(dataptr);
}

template<typename T1>
struct two_con {
    bool value1, value2;

    two_con() {
        value1 = std::is_pointer_v<T1>;
    }

    template<typename _T = T1, typename std::enable_if_t<std::is_same_v<_T, int>, int> = 0>
    void do_the_int_thing() {
        cout << "the int thing\n";
    }
};


void TB::implicit_two_con() {
    vector<int> banana;

    //two_con<int*>().do_the_int_thing();
}

struct jail_struct {
    static constexpr char *name = "Bob";
    vector<int> years_left;

};

void TB::struct_scope_escape() {
    cout << jail_struct::name;
}


// my_template.h
template<typename T>
struct my_template {
    static int brain_size;

    my_template() {
        brain_size++;
    }
};

template<typename T>
int my_template<T>::brain_size = 0;

template<typename T>
struct my_templ {
    // Genius logic: define it in the same file -> "must be defined out of line" -> define it in a separate file
    // -> "undefined reference"
    // Solution: define it the same file BUT LOWER *big brain stuff*
    // Logical conclusion: The C++ Standard eats memes and sometimes they pass undigested
    inline static int instance = 0;

    my_templ() {
        cout << "This is the most brain dead undefined reference I've ever seen: << " << instance++ << " \n";
    }
};

//template<typename T>
//int my_templ<T>::instance = 0;

/*
template<typename T>
struct my_templ_macro_ready;
template<typename T>
int my_templ_macro_ready<T>::instance = 0;
template<typename T>
struct my_templ_macro_ready{
    static int instance;
    my_templ_macro_ready(){
        cout << "This is the most brain dead undefined reference I've ever seen: << " <<  instance++ << " \n";
    }
};
 */



void TB::template_inconsitencies() {
    my_templ<int> i1;
    my_templ<int> i2;
    my_templ<char> c1;
    my_templ<int> i3;
    my_templ<char> c2;
};


size_t tunc_2p_down(size_t x) {
    size_t res = x >> 1;
    res |= res >> 1;
    res |= res >> 2;
    res |= res >> 4;
    res |= res >> 8;
    res |= res >> 16;
    res |= res >> 32;
    return x & ~res;
}

void TB::trunc_pow2() {
    for (size_t i = 2; i < 2048;) {
        cout << (tunc_2p_down(i) >> 1) << " < " << i << "\n";
        i += tunc_2p_down(i) >> 1;
    }
}

void TB::dynamic_queue() {
    std::list<int> queue;
    queue.push_back(1);
    for (auto &cur : queue) {
        if (cur < 1000)
            queue.push_back(cur * 2);
        cout << cur << "\n";
    }
}









