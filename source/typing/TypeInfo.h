//
// Created by david on 27/03/2020.
//

#ifndef GRAPHKAT_TYPEINFO_H
#define GRAPHKAT_TYPEINFO_H

#include "iterator.h"
#include "templates.h"
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

    struct TypeInfo {

        template<typename T>
        explicit
        TypeInfo(_internal::tag<T> t) :create(_internal::allocate<T>), copy(_internal::copy<T>), hasIterator(false),
                                       size(_internal::size_sum<T>), toString(_internal::toString<T>) {}

        TypeInfo(t_create f_create, t_copy f_copy, bool has_iterator, t_size f_size, t_toString f_toString) :
                create(f_create), copy(f_copy), hasIterator(has_iterator),
                size(f_size), toString(f_toString) {}

        t_create create;
        t_copy copy;

        bool hasIterator;

        t_size size;
        t_toString toString;

        template<typename T>
        static TypeInfo with_toString(t_toString toString) {
            return TypeInfo(_internal::allocate<T>, _internal::copy<T>, false, _internal::size_sum<T>, toString);
        }
    };

    template<typename T, t_iterator<T> f_iterator, t_access<T> f_access>
    class TypeAccess {

    private:
        t_access<T> _access = f_access;
    public:
        void *mem;

        TypeAccess(void *ptr) : mem(ptr) {}

        t_iterator<T> iterator = f_iterator;

        TypeAccess &operator=(T &other) {
            *_access(mem) = other;
            return *this;
        }

        TypeAccess &operator=(T other) {
            *_access(mem) = other;
            return *this;
        }

        operator T() {
            return *_access(mem);
        }

        operator void *() {
            return mem;
        }

        T *operator->() {
            return _access(mem);
        }

        T *value() {
            return _access(mem);
        }

    };
}


#endif //GRAPHKAT_TYPEINFO_H
