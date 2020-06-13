//
// Created by david on 12/06/2020.
//

#pragma once

#include <glm/glm.hpp>

using namespace glm;

template<typename T>
class oct_tree {
private:
    vec3 mini, maxi;
    struct Node {
        Node *children[8];
        T *value;
    };
    Node root;

    Node *findValue(Node *node, vec3 value, vec3 mini, vec3 maxi) {
        vec3 mid = (mini + maxi) / 2.0f;

        int index = 0;

        index += value.x < mid.x ? 1 : 0;
        index += value.y < mid.y ? 2 : 0;
        index += value.z < mid.z ? 4 : 0;

        /*if(node->children[index] == nullptr)
            return node;
        else return findValue(node->children[index], value, )*/

    }

public:
    oct_tree(vec3 mini, vec3 maxi);

    void put(T &value) {

    }
};

template<typename T>
oct_tree<T>::oct_tree(vec3 mini, vec3 maxi): mini(mini), maxi(maxi) {
    root = {{nullptr, nullptr, nullptr, nullptr}, nullptr};
}



