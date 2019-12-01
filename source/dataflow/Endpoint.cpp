//
// Created by david on 01/12/2019.
//

#include "Endpoint.h"

Endpoint::Endpoint() : value(nullptr), hasChanged(false), listeners(), linkedEndpoints() {}

Endpoint &Endpoint::operator=(any &other) {
    this->value = other;
    handleOnChanged();
    for (const auto &endpoint : linkedEndpoints)
        endpoint->handleOnChanged();
    return *this;
}

Endpoint::operator any &() {
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

void Endpoint::unregisterListener(void (*delegate)(any)) {
    listeners.push_back(delegate);
}

void Endpoint::registerListener(void (*delegate)(any)) {
    listeners.erase(std::remove(listeners.begin(), listeners.end(), delegate), listeners.end());
}

void Endpoint::handleOnChanged() {
    hasChanged = true;
    for (const auto &handler : listeners)
        handler(value);
}

