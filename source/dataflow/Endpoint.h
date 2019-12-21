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

class Endpoint {
private:
    boost::any _value;

    vector<ComputeNode *> listeners;
    vector<Endpoint *> slaveLinks;
    Endpoint *masterLink;

    void handleOnChanged();

public:
    enum EndpointType : short;
    static map<EndpointType, boost::any (*)(void)> initializers;

    bool hasChanged;
    EndpointType type;

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
        auto t = std::string(_value.type().name());
        return boost::any_cast<T>(this->_value);
    }

    //TODO: implement reference transparent behaviour
    void registerLink(Endpoint *endpoint);

    void unregisterLink(Endpoint *endpoint);

    void unregisterListener(ComputeNode *);

    void registerListener(ComputeNode *);

};

#endif //GRAPHKAT_ENDPOINT_H
