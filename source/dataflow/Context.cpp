//
// Created by david on 22/11/2019.
//

#include "Context.h"
#include "ContextPath.h"
#include <algorithm>
#include "messages.h"

Context *Context::GlobalRootContext = nullptr;
Context *Context::CurrentContext = nullptr;
string Context::CurrentPath = "/";

template<typename T>
void Context::createEndpoint(const string &name, T init_value) {
    endpoints[name] = Endpoint();
    endpoints[name] = init_value;
}

void Context::createEndpoint(const string &name) {
    endpoints[name] = Endpoint();
}

void Context::removeEndpoint(const string &name) {
    endpoints.erase(name);
}

Context::Context() : endpoints(), context_name(""), children() {
    if (Context::GlobalRootContext == nullptr) {
        Context::GlobalRootContext = this;
        Context::CurrentPath = "/";
        Context::CurrentContext = this;
    }
}

Context::Context(const string &name) : endpoints(), context_name(name), children() {}

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

/* Context path must begin and end with / endpoints end without */
Context* Context::createContext(const string &path) {

    auto [current_path, is_absolute] = ContextPath::makeRelative(path);
    Context *current_context = is_absolute? Context::GlobalRootContext : Context::CurrentContext;

    while (!current_path.empty()) {
        auto[head, tail] = ContextPath::getHeadTail(current_path);

        if (!current_context->children.count(head))
            current_context->children[head] = new Context(head);

        current_context = current_context->children[head];
        current_path = tail;
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

void Context::linkContext(Context &context) {
    throw;
}

void Context::unlinkContext(Context &context) {
    throw;
}

void Context::adoptContext(Context &context) {
    this->children[context.context_name] = &context;
}

void Context::disownContext(Context &context) {
    throw;
}




