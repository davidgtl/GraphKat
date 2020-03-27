//
// Created by david on 27/03/2020.
//

#ifndef GRAPHKAT_TYPEINFO_H
#define GRAPHKAT_TYPEINFO_H

#include "iterator.h"
#include <string>

namespace FancyTypes {
    using std::string;

    typedef void *(*t_create)();

    typedef void (*t_copy)(void *dest, void *src);

    template<typename T>
    using t_iterator = iterator<T> (*)();

    template<typename T>
    using t_read = T (*)();

    template<typename T>
    using t_write = void (*)(T &);

    template<typename T>
    using t_access= T *(*)(void *);

    typedef size_t (*t_size)();

    typedef string (*t_toString)(void *);

    template<typename T, t_create f_create, t_copy f_copy, bool has_iterator, t_iterator<T> f_iterator,
            t_access<T> f_access, t_size f_size, t_toString f_toString>
    struct TypeInfo {


        TypeInfo() : create(f_create), copy(f_copy), hasIterator(has_iterator),
                     iterator(f_iterator), access(f_access), size(f_size), toString(f_toString) {}

        t_create create;
        t_copy copy;

        bool hasIterator;

        t_iterator<T> iterator;

        //t_read<T> read;

        //t_write<T> write;

        t_access<T> access;

        t_size size;
        t_toString toString;
    };
}


#endif //GRAPHKAT_TYPEINFO_H
