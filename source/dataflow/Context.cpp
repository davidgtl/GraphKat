//
// Created by david on 22/11/2019.
//

#include "Context.h"
#include "ContextPath.h"
#include <algorithm>
#include <utils/Randoms.h>
#include "messages.h"
#include <utils/std_colors.h>


Context *Context::Root = new Context("root");
Context *Context::CurrentContext = Context::Root;
string Context::CurrentPath = "/";

//TODO: add destructor
Endpoint *Context::createEndpoint(const string &name) {
    if (master != nullptr) return nullptr; //Gatekeeper of chaos

    _endpoints[name] = new Endpoint();
    return _endpoints[name];
}

void Context::removeEndpoint(const string &name) {
    _endpoints.erase(name);
}

bool Context::hasEndpoint(const string &name) {
    return _endpoints.count(name) > 0;
}

Endpoint *Context::endpoint(const string &path) {

    auto[current_path, endpoint] = ContextPath::getPathEndpoint(path);
    Context *current_context = master == nullptr ? this : master;

    while (!current_path.empty()) {
        auto[head, tail] = ContextPath::getHeadTail(current_path);

        if (!current_context->children.count(head)) {
            current_path = head;
            fatal_error("Error: Context not found");
            break;
        }

        current_context = current_context->children[head];
        if (current_context->master != nullptr)
            current_context = current_context->master;
        current_path = tail;
    }

    if (!current_context->_endpoints.count(endpoint))
        return nullptr;

    return current_context->_endpoints[endpoint];
}

Context::Context() : _endpoints(), context_name(Randoms::random_string()), children() {
}

Context::Context(const string &name) : _endpoints(), context_name(name), children() {}

Context *Context::context(const string &path) {
    auto[current_path, is_absolute] = ContextPath::makeRelative(path);
    Context *current_context = master == nullptr ? this : master;
    //is_absolute ? Context::Root : Context::CurrentContext;

    while (!current_path.empty()) {
        auto[head, tail] = ContextPath::getHeadTail(current_path);

        if (!current_context->children.count(head))
            fatal_error("Context not found.");

        current_context = current_context->children[head];
        if (current_context->master != nullptr)
            current_context = current_context->master;
        current_path = tail;
    }

    return current_context;
}

/* Context path must begin and end with / endpoints end without */
Context *Context::createContext(const string &path) {

    auto[current_path, is_absolute] = ContextPath::makeRelative(path);
    Context *current_context = is_absolute ? Context::Root : Context::CurrentContext;

    while (!current_path.empty()) {
        auto[head, tail] = ContextPath::getHeadTail(current_path);

        if (!current_context->children.count(head))
            current_context->children[head] = new Context(head);

        current_context = current_context->children[head];
        if (current_context->master != nullptr)
            current_context = current_context->master;
        current_path = tail;
    }

    return current_context;
}

Context *Context::createSubContext(const string &name) {
    if (master != nullptr) return nullptr; //Gatekeeper of chaos

    this->children[name] = new Context(name);
    return this->children[name];
}

/* Should be treated as read only link unless specifically intended otherwise */
void Context::linkContext(Context *context) {
    this->master = context;
}

void Context::unlinkContext() {
    this->master = nullptr;
}

void Context::adoptContext(Context *context) {
    this->children[context->context_name] = context;
}

void Context::disownContext(Context *context) {
    this->children.erase(context->context_name);
}

void Context::pretty_print(int level) {
    using std::cout, std::endl;

    string cpath = "./";
    if (master != nullptr)
        cpath = "->" + FOREMAG + master->context_name + RST + "/";
    Context *ctx = master == nullptr ? this : master;
    for (const auto &e : ctx->_endpoints) {
        for (int i = 0; i < level; i++)
            cout << "|   ";
        cout << cpath << FOREBLU + e.first + RST << " = " << FOREGRN << e.second << RST << endl;
    }
    for (const auto &c : ctx->children) {
        for (int i = 0; i < level; i++)
            cout << "|   ";
        cout << cpath << FORERED << c.first << RST << "/" << endl;
    }

    for (const auto &c : ctx->children) {
        for (int i = 0; i < level; i++)
            cout << "|   ";
        cout << endl;
        for (int i = 0; i < level; i++)
            cout << "|   ";
        cout << cpath << FORERED << c.first << RST << "/:" << endl;
        c.second->pretty_print(level + 1);
    }

}


