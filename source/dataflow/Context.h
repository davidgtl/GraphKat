//
// Created by david on 22/11/2019.
//

#ifndef GRAPHKAT_CONTEXT_H
#define GRAPHKAT_CONTEXT_H

#include <string>
#include <boost/any.hpp>
#include <unordered_map>
#include "Endpoint.h"

#define CGV(ctx, name) ctx->context(#name)
#define CCV(ctx, name) ctx->createSubContext(#name)
#define CIV(name) in_ctx->context(#name)
#define COV(name) out_ctx->context(#name)

using boost::any;
using std::unordered_map, std::string, std::ostream;

class Endpoint;

template<typename Container>
class ConstIteratorProxy {
public:
    ConstIteratorProxy(const Container &container) : container_(container) {}

    typename Container::const_iterator begin() const {
        return container_.begin();
    };

    typename Container::const_iterator end() const {
        return container_.end();
    };
private:
    const Container &container_;
};

class Context {
private:
    unordered_map<string, Endpoint *> _endpoints;
    unordered_map<string, Context *> children;
    string context_name;
    Context *master = nullptr;

public:
    /*
     * What I want from you:
     * get a endpoint pointer from a path
     * set and endpoint from a path
     * link an endpoint to one from another context
     * register/remove a listener to an endpoint
     *
     * What I want you to do:
     *
     */

    /* Will be initialized with the first created Context */
    static Context *Root;
    static Context *CurrentContext;
    static string CurrentPath;

    Context();

    explicit Context(const string &name);

    template<typename T>
    Endpoint *createEndpoint(const string &name, T init_value) {
        _endpoints[name] = new Endpoint();
        _endpoints[name]->update(init_value);
    }

    /* Initialized to null */
    Endpoint *createEndpoint(const string &name);

    void removeEndpoint(const string &name);

    bool hasEndpoint(const string &name);

    void linkContext(Context *context);

    void unlinkContext();

    void adoptContext(Context *context);

    void disownContext(Context *context);

    /* Path must end with /context/ */
    Context *context(const string &path);

    /* Path must end with /endpoint */
    Endpoint *endpoint(const string &path);

    static Context *createContext(const string &path);

    Context *createSubContext(const string &path);

    friend ostream &operator<<(ostream &os, const Context &c) {
        return os << c.context_name;
    }

    auto endpoints() const {
        return ConstIteratorProxy(_endpoints);
    }

    auto contexts() const {
        return ConstIteratorProxy(children);
    }

    void pretty_print(int level = 0);

};

#endif //GRAPHKAT_CONTEXT_H
