//
// Created by david on 01/12/2019.
//

#ifndef GRAPHKAT_ENDPOINT_H
#define GRAPHKAT_ENDPOINT_H

#include <string>
#include <map>
#include <typeindex>
#include <boost/any.hpp>
#include <iostream>
#include "ComputeNode.h"
#include "nodeprims/macro_shenanigans.h"

using std::vector, std::map;

class ComputeNode;

//Potential problem: IDE autoformats paths with spaces around /


//get & set value
#define _EGV2(end_name, type) context->endpoint(#end_name)->value<type>()
#define _EGV3(context, end_name, type) context->endpoint(#end_name)->value<type>()
#define EGV(...) CAT(_EGV,COUNT_ARGUMENTS(__VA_ARGS__))(__VA_ARGS__)
#define _ESV3(end_name, type, val) context->endpoint(#end_name)->update<type>(val)
#define _ESV4(context, end_name, type, val) context->endpoint(#end_name)->update<type>(val)
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

};

#endif //GRAPHKAT_ENDPOINT_H
