//
// Created by david on 01/12/2019.
//

#ifndef GRAPHKAT_ENDPOINT_H
#define GRAPHKAT_ENDPOINT_H

#include <string>
#include <map>
#include <boost/any.hpp>

using std::vector;
using boost::any;

class Endpoint {
    any value;

    void handleOnChanged();

public:
    bool hasChanged;
    vector<void (*)(any)> listeners;
    vector<Endpoint *> linkedEndpoints;

    Endpoint();

    Endpoint &operator=(any &other);

    operator any &();

    void registerLink(Endpoint &endpoint);

    void unregisterLink(Endpoint &endpoint);

    void unregisterListener(void (*delegate)(any));

    void registerListener(void (*delegate)(any));
};

#endif //GRAPHKAT_ENDPOINT_H
