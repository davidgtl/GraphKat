//
// Created by david on 01/12/2019.
//

#ifndef GRAPHKAT_ENDPOINT_H
#define GRAPHKAT_ENDPOINT_H

#include <string>
#include <map>
#include <boost/any.hpp>

using std::vector, std::map;

class Endpoint {
private:
    boost::any value;

    vector<void (*)(boost::any)> listeners;
    vector<Endpoint *> linkedEndpoints;
    void handleOnChanged();

public:
    enum EndpointType : short;
    static map<EndpointType, boost::any (*)(void)> initializers;

    bool hasChanged;
    EndpointType type;

    Endpoint();

    void update(boost::any other);

    operator boost::any &();

    void registerLink(Endpoint &endpoint);

    void unregisterLink(Endpoint &endpoint);

    void unregisterListener(void (*delegate)(boost::any));

    void registerListener(void (*delegate)(boost::any));

};

#endif //GRAPHKAT_ENDPOINT_H
