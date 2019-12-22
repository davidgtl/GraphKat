//
// Created by david on 22/11/2019.
//

#ifndef GRAPHKAT_CONTEXT_H
#define GRAPHKAT_CONTEXT_H

#include <string>
#include <boost/any.hpp>
#include "Endpoint.h"

using boost::any;
using std::unordered_map, std::string, std::ostream;

class Endpoint;

class Context {
private:
    unordered_map<string, Endpoint *> endpoints;
    unordered_map<string, Context *> children;
    string context_name;

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

    Context(const string &name);

    template<typename T>
    void createEndpoint(const string &name, T init_value) {
        endpoints[name] = new Endpoint();
        endpoints[name]->update(init_value);
    }

    /* Initialized to null */
    Endpoint * createEndpoint(const string &name);

    void removeEndpoint(const string &name);

    void linkContext(Context &context);

    void unlinkContext(Context &context);

    void adoptContext(Context &context);

    void disownContext(Context &context);

    /* Path must end with /context/ */
    Context *path(string &path);

    /* Path must end with /endpoint */
    Endpoint *endpoint(const string &path);

    static Context *createContext(const string &path);

    friend ostream &operator<<(ostream &os, const Context &c) {
        return os << c.context_name;
    }
};

#endif //GRAPHKAT_CONTEXT_H
