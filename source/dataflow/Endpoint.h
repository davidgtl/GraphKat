//
// Created by david on 01/12/2019.
//

#ifndef GRAPHKAT_ENDPOINT_H
#define GRAPHKAT_ENDPOINT_H

#include <string>
#include <map>
#include <boost/any.hpp>
#include <iostream>

using std::vector, std::map;

class Endpoint {
private:
    boost::any _value;

    vector<void (*)(boost::any)> listeners;
    vector<Endpoint *> linkedEndpoints;
    void handleOnChanged();

public:
    enum EndpointType : short;
    static map<EndpointType, boost::any (*)(void)> initializers;

    bool hasChanged;
    EndpointType type;

    Endpoint();

    template<typename T>
    void update(T val) {
        this->_value = val;
        handleOnChanged();
        for (const auto &endpoint : linkedEndpoints)
            endpoint->handleOnChanged();
    }

    template<typename T>
    T value() {
        hasChanged = false;
        auto t = std::string(_value.type().name());
        return boost::any_cast<T>(this->_value);
    }

    void registerLink(Endpoint &endpoint);

    void unregisterLink(Endpoint &endpoint);

    void unregisterListener(void (*delegate)(boost::any));

    void registerListener(void (*delegate)(boost::any));

};

#endif //GRAPHKAT_ENDPOINT_H
