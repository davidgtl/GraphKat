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
        T *data;
        vec3 value;

        Node() {
            for (int i = 0; i < 8; i++) children[i] = nullptr;
            data = nullptr;
        }
    };

    Node root;

    int index(vec3 &value, vec3 &mid) {
        int index = 0;

        index += value.x < mid.x ? 1 : 0;
        index += value.y < mid.y ? 2 : 0;
        index += value.z < mid.z ? 4 : 0;

        return index;
    }

    void child_range(vec3 &value, vec3 &mid, vec3 &child_min, vec3 &child_max) {
        if (value.x < mid.x) child_max.x = mid.x;
        else child_min.x = mid.x;

        if (value.y < mid.y) child_max.y = mid.y;
        else child_min.y = mid.y;

        if (value.z < mid.z) child_max.z = mid.z;
        else child_min.z = mid.z;
    }

    Node *find_value(Node *node, vec3 value, vec3 mini, vec3 maxi) {
        vec3 mid = (mini + maxi) / 2.0f;

        int index = this->index(value, mid);

        vec3 child_min(mini);
        vec3 child_max(maxi);
        child_range(value, mid, child_min, child_max);

        if (node->children[index] == nullptr)
            return node;
        else return findValue(node->children[index], value, child_min, child_max);

    }

    void put_value(T &data, Node *node, vec3 value, vec3 mini, vec3 maxi) {
        vec3 mid = (mini + maxi) / 2.0f;

        int index = this->index(value, mid);

        vec3 child_min(mini);
        vec3 child_max(maxi);
        child_range(value, mid, child_min, child_max);

        if (node->children[index] == nullptr) {
            bool ok = true;
            for (int i = 0; i < 8; i++) {
                if (i == index) continue;
                if (node->children[i] != nullptr)
                    ok = false;
            }
            if (!ok) {
                node->children[index] = new Node();
                node->children[index]->data = &data;
            } else {
                if (node->data != nullptr) {
                    int other_index = this->index(node->value, mini, maxi);
                    node->children[other_index] = new Node();
                    node->children[other_index]->data = node->data;
                    node->children[other_index]->value = node->value;

                    node->children[index] = new Node();
                    node->children[index]->data = &data;
                    node->children[index]->value = value;

                } else {
                    node->data = data;
                    node->value = value;
                }
            }
            return;
        } else put_value(data, node->children[index], value, child_min, child_max);
    }

public:
    oct_tree(vec3 mini, vec3 maxi);

    void put(T &data, vec3 value) {
        put_value(data, root, value, mini, maxi);
    }

    void find(vec3 value, T &data, vec3 &actual_value) {
        Node *n = find_value(root, value, mini, maxi);
        data = *n->data;
        actual_value = *n->value;
    }
};

template<typename T>
oct_tree<T>::oct_tree(vec3 mini, vec3 maxi): mini(mini), maxi(maxi) {
}



