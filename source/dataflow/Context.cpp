//
// Created by david on 22/11/2019.
//

#include "Context.h"
#include "ContextPath.h"
#include <algorithm>
#include "messages.h"

void Context::setContext(const string &name) {
    context_name = name;
}

void Context::removeContext() {
    context_name = "";
}

template<typename T>
void Context::createEndpoint(const string &name, T default_value) {
    endpoints[name] = Endpoint();
    endpoints[name] = default_value;
}

void Context::createEndpoint(const string &name) {
    endpoints[name] = Endpoint();
}

void Context::removeEndpoint(const string &name) {
    endpoints.erase(name);
}

Context::Context() : endpoints(), context_name(""), children() {
    if (Context::GlobalContext == nullptr) {
        Context::GlobalContext = this;
    }
}

Context::Context(string &name) : endpoints(), context_name(name), children() {}

Context *Context::path(string &path) {
    Context *current_context = nullptr;
    while (true) {
        auto[head, tail] = ContextPath::getHeadTail(path);

        if (!children.count(head))
            break;

        current_context = children[head];
    }

    return current_context;
}

Endpoint* Context::getEndpoint(const string &path) {

    auto[current_path, endpoint] = ContextPath::getPathEndpoint(path);
    Context *current_context = nullptr;

    while (true) {
        auto[head, tail] = ContextPath::getHeadTail(current_path);

        if (!children.count(head)) {
            current_path = head;
            break;
        }

        current_context = children[head];
        current_path = tail;
    }

    if (!current_context->endpoints.count(path))
        return nullptr;

    return &current_context->endpoints[path];
}

Endpoint* Context::operator[](string &path) {
    return getEndpoint(path);
}



