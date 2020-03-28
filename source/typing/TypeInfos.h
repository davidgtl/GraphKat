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

namespace FancyTypes {

    using std::string;
    using namespace _internal;

    template<typename T>
    using primitive_type = TypeAccess<T, nullptr, access<T>>;


    typedef primitive_type<int> int_t;
    typedef primitive_type<unsigned int> uint_t;
    typedef primitive_type<float> float_t;
    typedef primitive_type<double> double_t;
    typedef primitive_type<string> string_t;


    TypeInfo int_i(tag<int>{});
    TypeInfo uint_i(tag<unsigned int>{});
    TypeInfo float_i(tag<float>{});
    TypeInfo double_i(tag<double>{});
    TypeInfo string_i(tag<string>{});

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