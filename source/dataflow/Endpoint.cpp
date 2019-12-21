//
// Created by david on 01/12/2019.
//

#include "Endpoint.h"

Endpoint::Endpoint() : _value(nullptr), hasChanged(false), listeners(), slaveLinks() {}

void Endpoint::registerLink(Endpoint *endpoint) {
    masterLink = endpoint;
    endpoint->slaveLinks.push_back(endpoint);
}

void Endpoint::unregisterLink(Endpoint *endpoint) {
    masterLink->slaveLinks.erase(std::remove(slaveLinks.begin(), slaveLinks.end(), endpoint),
                                 slaveLinks.end());
    masterLink = nullptr;
}

void Endpoint::registerListener(ComputeNode *node) {
    listeners.push_back(node);
}

void Endpoint::unregisterListener(ComputeNode *node) {
    listeners.erase(std::remove(listeners.begin(), listeners.end(), node), listeners.end());
}

void Endpoint::handleOnChanged() {
    hasChanged = true;
    for (const auto &handler : listeners)
        handler->execute();
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

//template void foo<int>();




