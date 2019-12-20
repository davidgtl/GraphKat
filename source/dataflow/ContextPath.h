//
// Created by david on 30/11/2019.
//

#ifndef GRAPHKAT_CONTEXTPATH_H
#define GRAPHKAT_CONTEXTPATH_H

#include <string>

using namespace std;

class ContextPath {
public:
    static string getHead(const string &path);

    static string getTail(const string &path);

    static tuple<string, string> getHeadTail(const string &path);

    static tuple<string, string> getPathEndpoint(const string &path);

    /* !relative = absolute */
    static bool isRelative(const string &path);

    static bool isAbsolute(const string &path);

    static tuple<std::string, bool> makeRelative(const string &path);

    static string getParent(const string &path);
};


#endif //GRAPHKAT_CONTEXTPATH_H
