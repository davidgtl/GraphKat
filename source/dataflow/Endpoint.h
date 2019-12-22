//
// Created by david on 01/12/2019.
//

#ifndef GRAPHKAT_ENDPOINT_H
#define GRAPHKAT_ENDPOINT_H

#include <string>
#include <map>
#include <boost/any.hpp>
#include <iostream>
#include "ComputeNode.h"

using std::vector, std::map;

class ComputeNode;

#define EIV(end_name, type) in_ctx->endpoint(#end_name)->value<type>()
#define EOV(end_name, type, val) out_ctx->endpoint(#end_name)->update(val)

class Endpoint {
private:
    boost::any _value;

    vector<ComputeNode *> listeners;
    vector<Endpoint *> slaveLinks;
    Endpoint *masterLink;

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
