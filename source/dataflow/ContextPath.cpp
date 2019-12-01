//
// Created by david on 30/11/2019.
//

#include "ContextPath.h"
#include <tuple>

using namespace std;

string ContextPath::getParent(const string &path) {
    std::size_t found = path.find_last_of('/');
    return path.substr(0, found);
}

string ContextPath::getHead(const string &path) {
    std::size_t found = path.find_first_of('/');
    return path.substr(0, found);
}

string ContextPath::getTail(const string &path) {
    std::size_t found = path.find_first_of('/');
    return path.substr(found + 1);
}

tuple<string, string> ContextPath::getHeadTail(const string &path) {
    std::size_t found = path.find_first_of('/');
    return {path.substr(0, found), path.substr(found + 1)};
}

tuple<string, string> ContextPath::getPathEndpoint(const string &path) {
    std::size_t found = path.find_last_of('/');
    return {path.substr(0, found), path.substr(found + 1)};
}
