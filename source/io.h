//
// Created by David Ghitulescu on 2019-07-24.
//
#pragma once

#include <fstream>
#include <string>

using namespace std;

inline string loadStringFromFile(const string &path) {
    ifstream t(path);
    string str((istreambuf_iterator<char>(t)),
               istreambuf_iterator<char>());
    return str;
}