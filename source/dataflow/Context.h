//
// Created by david on 22/11/2019.
//

#ifndef GRAPHKAT_CONTEXT_H
#define GRAPHKAT_CONTEXT_H

#include <string>
#include <map>
#include <boost/any.hpp>
#include "Endpoint.h"

using boost::any;
using std::map, std::string;

class Context {
private:
    map<string, Endpoint> endpoints;
    map<string, boost::any *> endpoint_ref;
    map<string, Context *> children;
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
    static Context *GlobalContext;

    Context();

    Context(string &name);

    template<typename T>
    void createEndpoint(const string &name, T default_value);

    /* Initialized to null */
    void createEndpoint(const string &name);


    void removeEndpoint(const string &name);

    void setContext(const string &name);

    void removeContext();

    Context *path(string &name);

    Endpoint *getEndpoint(const string &path);

    /* Path must end with an endpoint */
    Endpoint *operator[](string &path);
};

Context *Context::GlobalContext = nullptr;

#endif //GRAPHKAT_CONTEXT_H
