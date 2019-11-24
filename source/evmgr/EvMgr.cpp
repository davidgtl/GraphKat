//
// Created by david on 22/11/2019.
//

#include "EvMgr.h"
#include <algorithm>

void EvMgr::setContext(const string &name) {
    context = name;
}

void EvMgr::removeContext() {
    context = "";
}

template<typename T>
void EvMgr::createEndpoint(const string &name, T default_value) {
    endpoints[context + name] = default_value;
    listeners[context + name] = vector<T>();
}

void EvMgr::removeEndpoint(const string &name) {
    endpoints.erase(context + name);
    listeners.erase(context + name);
}

template<typename T>
void EvMgr::registerListener(const string &name, void (*handler)(T)) {
    listeners[context + name].push_back(handler);
}

template<typename T>
void EvMgr::removeListener(const string &name, void (*handler)(T)) {
    listeners[context + name].erase(remove(listeners[context + name].begin(), listeners[context + name].end(), handler),
                                    listeners[context + name].end());
}

template<typename T>
void EvMgr::setValue(const string &name, T value) {
    endpoints[context + name] = value;
    for (const auto &handler : listeners[context + name])
        handler(value);
}

template<typename T>
T EvMgr::getValue(const string &name) {
    return endpoints[context + name];
}

EvMgr::EvMgr():endpoints (), listeners(), context("") {}

