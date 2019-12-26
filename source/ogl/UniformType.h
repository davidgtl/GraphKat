//
// Created by david on 01/12/2019.
//

#ifndef GRAPHKAT_UNIFORMTYPE_H
#define GRAPHKAT_UNIFORMTYPE_H

#include <boost/any.hpp>
#include <glad/glad.h>

class UniformType {
public:
        GLenum type;
    int count;
    int location;

        boost::any getInit();

        static std::unordered_map<GLenum, boost::any> initializers;

    UniformType(GLenum type, int size, int location);
};


#endif //GRAPHKAT_UNIFORMTYPE_H
