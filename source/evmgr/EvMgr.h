//
// Created by david on 22/11/2019.
//

#ifndef GRAPHKAT_EVMGR_H
#define GRAPHKAT_EVMGR_H

#include <string>
#include <map>
#include <boost/any.hpp>

using namespace std;

class EvMgr {
private:
    map<string, boost::any> endpoints;
    map<string, vector<boost::any>> listeners;
    string context;

public:

    EvMgr();

    template <typename T>
    void createEndpoint(const string& name, T default_value);

    void removeEndpoint(const string& name);

    template <typename T>
    void setValue(const string &name, T value);

    template <typename T>
    T getValue(const string &name);

    template<typename T>
    void removeListener(const string &name, void (*handler)(T));

    template<typename T>
    void registerListener(const string &name, void (*handler)(T));

    void setContext(const string &name);

    void removeContext();
};

EvMgr gEvMgr;

#endif //GRAPHKAT_EVMGR_H
