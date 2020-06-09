//
// Created by david on 26/12/2019.
//

#ifndef GRAPHKAT_SHADERS_H
#define GRAPHKAT_SHADERS_H

#include <ogl/UniformType.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cassert>
#include <vector>

namespace Shaders {
    using namespace glm;
    using namespace std;

    template<typename T>
    inline void bind_uniform(UniformType type, T value);

    template<>
    inline void bind_uniform(UniformType type, float value) {
        assert(type.type == GL_FLOAT && type.count == 1);
        glUniform1f(type.location, value);
    }

    template<>
    inline void bind_uniform(UniformType type, vector<float> buffy) {
        assert(type.type == GL_FLOAT && type.count == buffy.size());
        glUniform1fv(type.location, buffy.size(), buffy.data());
    }

    template<>
    inline void bind_uniform(UniformType type, vec2 value) {
        assert(type.type == GL_FLOAT_VEC2 && type.count == 1);
        glUniform2fv(type.location, 1, value_ptr(value));
    }

    template<>
    inline void bind_uniform(UniformType type, vector<vec2> buffy) {
        assert(type.type == GL_FLOAT_VEC2 && type.count == buffy.size());
        glUniform2fv(type.location, buffy.size(), value_ptr(buffy[0]));
    }

    template<>
    inline void bind_uniform(UniformType type, vec3 value) {
        assert(type.type == GL_FLOAT_VEC3 && type.count == 1);
        glUniform3fv(type.location, 1, value_ptr(value));
    }

    template<>
    inline void bind_uniform(UniformType type, vector<vec3> buffy) {
        assert(type.type == GL_FLOAT_VEC3 && type.count == buffy.size());
        glUniform3fv(type.location, buffy.size(), value_ptr(buffy[0]));
    }

    template<>
    inline void bind_uniform(UniformType type, vec4 value) {
        assert(type.type == GL_FLOAT_VEC4 && type.count == 1);
        glUniform4fv(type.location, 1, value_ptr(value));
    }

    template<>
    inline void bind_uniform(UniformType type, vector<vec4> buffy) {
        assert(type.type == GL_FLOAT_VEC4 && type.count == buffy.size());
        glUniform4fv(type.location, buffy.size(), value_ptr(buffy[0]));
    }

    template<>
    inline void bind_uniform(UniformType type, int value) {
        assert(type.type == GL_INT && type.count == 1);
        glUniform1i(type.location, value);
    }


}
/*
 * #define GL_INT_VEC2 0x8B53
 * #define GL_INT_VEC3 0x8B54
 * #define GL_INT_VEC4 0x8B55
 * #define GL_FLOAT_MAT2 0x8B5A
 * #define GL_FLOAT_MAT3 0x8B5B
 * #define GL_FLOAT_MAT4 0x8B5C
 * #define GL_INT 0x1404
 * #define GL_UNSIGNED_BYTE 0x1401
 * #define GL_SHORT 0x1402
 * #define GL_UNSIGNED_SHORT 0x1403
 * #define GL_INT 0x1404
 * #define GL_UNSIGNED_INT 0x1405
 * #define GL_BOOL 0x8B56
 *
 * #define GL_BOOL_VEC2 0x8B57
 * #define GL_BOOL_VEC3 0x8B58
 * #define GL_BOOL_VEC4 0x8B59
 */

#endif //GRAPHKAT_SHADERS_H
