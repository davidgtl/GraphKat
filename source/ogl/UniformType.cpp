//
// Created by david on 01/12/2019.
//

#include "UniformType.h"
#include <glad/glad.h>

using any = boost::any;

std::unordered_map<GLenum, boost::any> UniformType::initializers={
        {GL_FLOAT, any(0.0)}
};

boost::any UniformType::getInit() {
    return initializers[type];
}

UniformType::UniformType(GLenum type, int size, int location) : type(type), size(size), location(location) {}
