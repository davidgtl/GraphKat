//
// Created by david on 01/12/2019.
//

#ifndef GRAPHKAT_ENDPOINT_H
#define GRAPHKAT_ENDPOINT_H

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>
#include <map>
#include <typeindex>
#include <boost/any.hpp>
#include <iostream>
#include "ComputeNode.h"
#include "nodeprims/macro_shenanigans.h"

using boost::any_cast;
using std::vector, std::map;
using glm::to_string, glm::vec2, glm::vec3, glm::vec4;

class ComputeNode;

//Potential problem: IDE autoformats paths with spaces around /


//get & set value
#define _EGV2(end_name, type) context->endpoint(#end_name)->value<type>()
#define _EGV3(context, end_name, type) context->endpoint(#end_name)->value<type>()
#define EGV(...) CAT(_EGV,COUNT_ARGUMENTS(__VA_ARGS__))(__VA_ARGS__)
#define _ESV3(context, end_name, val) context->endpoint(#end_name)->update(val)
#define ESV(...) CAT(_ESV,COUNT_ARGUMENTS(__VA_ARGS__))(__VA_ARGS__)
//special cases
#define EIV(end_name, type) in_ctx->endpoint(#end_name)->value<type>()
#define EE(context, function_name) context->endpoint(#function_name)->value<ComputeNode*>()->execute()
#define EOV(end_name, type, val) out_ctx->endpoint(#end_name)->update(val)
#define ECV(context, name, value) context->createEndpoint(#name, value)

class Endpoint {
private:
    boost::any _value;

    vector<ComputeNode *> listeners;
    vector<Endpoint *> slaveLinks;
    Endpoint *masterLink = nullptr;

    void handleOnChanged();

public:
    bool hasChanged;

    Endpoint();

    template<typename T>
    void update(T val) {
        if (masterLink != nullptr) {
            masterLink->_value = val;
            masterLink->handleOnChanged();
        }
        this->_value = val;
        handleOnChanged();
        for (const auto &endpoint : slaveLinks)
            endpoint->handleOnChanged();
    }

    void update() {
        if (masterLink != nullptr)
            masterLink->handleOnChanged();

        handleOnChanged();
        for (const auto &endpoint : slaveLinks)
            endpoint->handleOnChanged();
    }

    template<typename T>
    T value() {
        hasChanged = false;
        return boost::any_cast<T>(this->_value);
    }

    std::type_index type() {
        return this->_value.type();
    }

    //TODO: implement reference transparent behaviour
    void registerLink(Endpoint *endpoint);

    void unregisterLink(Endpoint *endpoint);

    void unregisterListener(ComputeNode *);

    void registerListener(ComputeNode *);

    friend std::ostream &operator<<(std::ostream &stream, const Endpoint *e) {
        if (e->_value.type() == typeid(float))
            stream << any_cast<float>(e->_value);
        else if (e->_value.type() == typeid(vec2))
            stream << to_string(any_cast<vec2>(e->_value));
        else if (e->_value.type() == typeid(vec3))
            stream << to_string(any_cast<vec3>(e->_value));
        else if (e->_value.type() == typeid(vec4))
            stream << to_string(any_cast<vec4>(e->_value));
        else if (e->_value.type() == typeid(int))
            stream << any_cast<int>(e->_value);
        else if (e->_value.type() == typeid(double))
            stream << any_cast<double>(e->_value);
        else
            stream << reinterpret_cast<const void *>(e);
        return stream;
    }

};

#endif //GRAPHKAT_ENDPOINT_H
