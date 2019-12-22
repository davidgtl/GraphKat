//
// Created by david on 22/11/2019.
//

#include "Context.h"
#include "ContextPath.h"
#include <algorithm>
#include "messages.h"

Context *Context::Root = new Context("root");
Context *Context::CurrentContext = Context::Root;
string Context::CurrentPath = "/";

//TODO: add destructor
Endpoint *Context::createEndpoint(const string &name) {
    endpoints[name] = new Endpoint();
    return endpoints[name];
}

void Context::removeEndpoint(const string &name) {
    endpoints.erase(name);
}

Context::Context() : endpoints(), context_name(""), children() {
}

Context::Context(const string &name) : endpoints(), context_name(name), children() {}

Context *Context::path(string &path) {
    auto[current_path, is_absolute] = ContextPath::makeRelative(path);
    Context *current_context = is_absolute ? Context::Root : Context::CurrentContext;

    while (!current_path.empty()) {
        auto[head, tail] = ContextPath::getHeadTail(current_path);

        if (!current_context->children.count(head))
            fatal_error("Context not found.");

        current_context = current_context->children[head];
        current_path = tail;
    }

    return current_context;
}

/* Context path must begin and end with / endpoints end without */
Context* Context::createContext(const string &path) {

    auto[current_path, is_absolute] = ContextPath::makeRelative(path);
    Context *current_context = is_absolute ? Context::Root : Context::CurrentContext;

    while (!current_path.empty()) {
        auto[head, tail] = ContextPath::getHeadTail(current_path);

        if (!current_context->children.count(head))
            current_context->children[head] = new Context(head);

        current_context = current_context->children[head];
        current_path = tail;
    }

    return current_context;
}

Endpoint *Context::endpoint(const string &path) {

    auto[_path, endpoint] = ContextPath::getPathEndpoint(path);
    auto[current_path, is_absolute] = ContextPath::makeRelative(_path);
    Context *current_context = is_absolute ? Context::Root : this;

    while (!current_path.empty()) {
        auto[head, tail] = ContextPath::getHeadTail(current_path);

        if (!current_context->children.count(head)) {
            current_path = head;
            fatal_error("Error: Context not found");
            break;
        }

        current_context = current_context->children[head];
        current_path = tail;
    }

    if (!current_context->endpoints.count(endpoint))
        return nullptr;

    int k = current_context->endpoints.count(endpoint);

    return current_context->endpoints[endpoint];
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




