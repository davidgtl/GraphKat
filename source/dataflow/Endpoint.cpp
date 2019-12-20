//
// Created by david on 01/12/2019.
//

#include "Endpoint.h"

Endpoint::Endpoint() : value(nullptr), hasChanged(false), listeners(), linkedEndpoints() {}

void Endpoint::update(boost::any other) {
    this->value = other;
    handleOnChanged();
    for (const auto &endpoint : linkedEndpoints)
        endpoint->handleOnChanged();
}

Endpoint::operator boost::any &() {
    hasChanged = false;
    return this->value;
}

void Endpoint::registerLink(Endpoint &endpoint) {
    linkedEndpoints.push_back(&endpoint);
}

void Endpoint::unregisterLink(Endpoint &endpoint) {
    linkedEndpoints.erase(std::remove(linkedEndpoints.begin(), linkedEndpoints.end(), &endpoint),
                          linkedEndpoints.end());
}

void Endpoint::unregisterListener(void (*delegate)(boost::any)) {
    listeners.push_back(delegate);
}

void Endpoint::registerListener(void (*delegate)(boost::any)) {
    listeners.erase(std::remove(listeners.begin(), listeners.end(), delegate), listeners.end());
}

void Endpoint::handleOnChanged() {
    hasChanged = true;
    for (const auto &handler : listeners)
        handler(value);
}

enum Endpoint::EndpointType : short {
    Float, FloatV,
    Vec2, Vec2V,
    Vec3, Vec3V,
    Vec4, Vec4V
};

using T = Endpoint::EndpointType;
using bany = boost::any;

map<T, boost::any (*)(void)> Endpoint::initializers = {
        {T::Float,  []() { return bany(float(0.0)); }},
        {T::FloatV, []() { return bany(vector<float>()); }}
};




