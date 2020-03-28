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

    TypeInfo int_i(tag<int>{});
    TypeInfo uint_i(tag<unsigned int>{});
    TypeInfo float_i(tag<float>{});
    TypeInfo double_i(tag<double>{});
    TypeInfo string_i(tag<string>{});

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
        string name = "";
        unsigned int count;
        unsigned int length;
        unsigned int type;

        uninfo operator+(uninfo &other) {
            return {this->name, this->count + other.count, this->length, this->type};
        }
    };

    TypeInfo uninfo_i = FancyTypes::TypeInfo::with_toString<uninfo>([](std::ostream &cout, void *ptr) {
        auto val = (uninfo *) ptr;
        cout << "{" << val->name << "," << val->count << "," << val->length << "," << val->type << "}";
    });

    typedef FancyTypes::primitive_type<uninfo, uninfo_i> uninfo_t;

    template<>
    class I<uninfo_t> {

    public:
        string *name;
        unsigned int *count;
        unsigned int *length;
        unsigned int *type;

        I(uninfo_t &t) : name(&t->name), count(&t->count), length(&t->length), type(&t->type) {}
    };



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