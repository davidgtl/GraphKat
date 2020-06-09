#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by david on 28/03/2020.
//

#ifndef GRAPHKAT_TYPEINFOS_H
#define GRAPHKAT_TYPEINFOS_H

#include "TypeInfo.h"
#include "templates.h"
#include <string>
#include <glm/glm.hpp>

namespace FancyTypes {

    using std::string;
    using namespace _internal;

    template<typename T>
    class I {
    public:
        I(T &t) {}
    };

    template<typename T, TypeInfo &typeinfo>
    using primitive_type = TypeAccess<T, nullptr, access<T>, typeinfo>;

    template<typename T>
    TypeInfo create_type() {
        return TypeInfo(_internal::allocate<T>, _internal::initialize<T>,
                _internal::copy<T>, false,
                _internal::size_sum<T>, _internal::toStream<T>);
    }

    //TypeInfo int_i<int>();
    //TypeInfo int_i(tag<int>{});
    auto int_i = create_type<int>();
    auto uint_i = create_type<unsigned int>();
    auto float_i = create_type<float>();
    auto double_i = create_type<double>();
    auto string_i = create_type<string>();

    TypeInfo vec3_i = FancyTypes::TypeInfo::with_toString<glm::vec3>([](std::ostream &cout, void *ptr) {
        auto val = (glm::vec3 *) ptr;
        cout << "{" << val->x << "," << val->y << "," << val->z << "}";
    });

    typedef primitive_type<int, int_i> int_t;
    typedef primitive_type<unsigned int, uint_i> uint_t;
    typedef primitive_type<float, float_i> float_t;
    typedef primitive_type<double, double_i> double_t;
    typedef primitive_type<string, string_i> string_t;
    typedef primitive_type<glm::vec3, vec3_i> vec3_t;


    struct uninfo {

        string_t name;
        uint_t count;
        uint_t length;
        uint_t type;

        uninfo() : name(), count(), length(), type() {}

        uninfo(const string &name, unsigned int count, unsigned int length, unsigned int type) :
                name(name), count(count), length(length), type(type) {}

        uninfo operator+(uninfo &other) {
            return {this->name, this->count + other.count, this->length, this->type};
        }
    };

    TypeInfo uninfo_i = FancyTypes::TypeInfo::with_toString<uninfo>([](std::ostream &cout, void *ptr) {
        auto val = (uninfo *) ptr;
        cout << "{" << val->name << "," << val->count << "," << val->length << "," << val->type << "}";
    });

    typedef FancyTypes::primitive_type<uninfo, uninfo_i> uninfo_t;


    /*
    template<>
    class I<uninfo_t> {

    public:
        string_t name;
        uint_t count;
        uint_t length;
        uint_t type;

        I(uninfo_t &t) : name(&t->name), count(&t->count), length(&t->length), type(&t->type) {}
    };*/



    /*
   primitive_type<int> int_t;
   primitive_type<unsigned int> uint_t;
   primitive_type<float> float_t;
   primitive_type<double> double_t;
   primitive_type<string> string_t;
   */

}

#endif //GRAPHKAT_TYPEINFOS_H

#pragma clang diagnostic pop