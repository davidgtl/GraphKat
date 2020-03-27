//
// Created by david on 27/03/2020.
//
#ifndef GRAPHKAT_templates_h
#define GRAPHKAT_templates_h

#include <string>
#include <iostream>
#include <sstream>

namespace FancyTypes::_internal {
    using std::stringstream, std::string;

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

    /*template<typename... Types>
    struct Typelist {
    };

    // Declaration of a template
    template<typename TypeListOne, typename TypeListTwo>
    void print(stringstream &s, void *mem);


    template <typename... TypesOne, typename... TypesTwo>
    using kk = void <Typelist<TypesOne...>, Typelist<TypesTwo>>(*)();


    template<typename T, template<typename... Offset> class C>
    void print<Typelist<TypeListOne...>>(stringstream &s, void *mem) {
        s << access<T, Offset>(mem);
    }

    template<typename First, template<typename...> class C, typename... Rest>
    void print(stringstream &s, void *mem, First firstValue, Rest... rest) {
        print(s, firstValue);
        s << ", ";
        print(rest...);
    }*/

    template<typename T>
    string toString(void *mem) {
        stringstream buffy;

        buffy << access<T>(mem);
        return buffy.str();
    }
}


#endif