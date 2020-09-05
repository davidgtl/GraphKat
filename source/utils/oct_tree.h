//
// Created by david on 12/06/2020.
//

#pragma once

#include <glm/glm.hpp>
#include <list>
#include <unordered_map>
#include <vector>

using namespace glm;

template<typename T>
class oct_tree {
private:
    vec3 mini, maxi;

    struct Node {
        Node *children[8];
        T data;
        vec3 value;
        bool has_data = false;
        bool has_ref_data = false;
        int ref_branch_index = -1;
        int branch_index = -1;
        int data_index = -1;
        vec3 bound_max;
        vec3 bound_min;

        Node() {
            for (int i = 0; i < 8; i++) children[i] = nullptr;
            has_data = false;
        }
    };


    int point_count;
    int split_count;
    int max_depth = 0;
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

    void child_range(int index, vec3 &mid, vec3 &child_min, vec3 &child_max) {
        if (index & 1) child_max.x = mid.x;
        else child_min.x = mid.x;

        if (index & 2) child_max.y = mid.y;
        else child_min.y = mid.y;

        if (index & 4) child_max.z = mid.z;
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

    void put_value(T data, Node *node, vec3 value, vec3 mini, vec3 maxi, int levels = 1) {
        if (glm::any(glm::isnan(value)))
            printf("Captain we have a problem 92");
        max_depth = glm::max(max_depth, levels);
        vec3 mid = (mini + maxi) / 2.0f;

        int index = this->index(value, mid);

        vec3 child_min(mini);
        vec3 child_max(maxi);
        child_range(index, mid, child_min, child_max);

        if(glm::any(glm::lessThan(value, child_min)) || glm::any(glm::greaterThan(value, child_max)) )
            printf("Captain we have a bounds problem 103");

        bool has_children = false;
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != nullptr)
                has_children = true;
        }

        if (!has_children && !node->has_data) {
            node->data = data;
            node->value = value;
            node->has_data = true;
        } else if (!has_children && node->has_data) {
            int other_index = this->index(node->value, mid);
            node->children[other_index] = new Node();
            node->children[other_index]->data = node->data;
            node->children[other_index]->has_data = true;
            node->children[other_index]->value = node->value;

            node->children[index] = new Node();
            node->children[index]->data = data;
            node->children[index]->has_data = true;
            node->children[index]->value = value;

            node->has_data = false;
            split_count++;
        } // by now it has children for sure
        else if (node->children[index] == nullptr) {
            node->children[index] = new Node();
            node->children[index]->data = data;
            node->children[index]->has_data = true;
            node->children[index]->value = value;
        } else
            put_value(data, node->children[index], value, child_min, child_max, levels + 1);
    }

    Node *
    fill_empties(const std::vector<Node *> &index_map) {

        for (int i = 0; i < index_map.size(); ++i) {
            Node *node = index_map[i];

            if (node->has_data)
                continue;

            float closest_dist = 1000.0f;

            for (int j = 0; j < index_map.size(); ++j) {
                if (!index_map[j]->has_data)
                    continue;

                float new_dist = glm::length(index_map[j]->value - node->value);
                if (new_dist < closest_dist) {
                    closest_dist = new_dist;
                    node->data_index = index_map[j]->data_index;
                    node->has_ref_data = true;
                }
            }
            if (node->data_index < 1)
                printf("Captain we have a problem 162");
        }
    }

public:
    struct branch_index {
        int branch_index[8];
        int data_index;
    };

    oct_tree(vec3 mini, vec3 maxi);

    void put(T data, vec3 value) {
        point_count++;
        put_value(data, &root, value, mini, maxi);
    }

    void find(vec3 value, T &data, vec3 &actual_value) {
        Node *n = find_value(&root, value, mini, maxi);
        data = n->data;
        actual_value = *n->value;
    }

    void generate_index_data(branch_index *&index_array, int &index_size, T *&data_array, int &data_size, int &levels) {
        using namespace std;

        //allocate index_array and data_array

        std::vector<Node *> index_map;

        index_map.push_back(&root);
        int curr_index = 0;
        int data_index = 0;

        // create index hashmap
        for (curr_index = 0; curr_index < index_map.size(); curr_index++) {
            Node *curr = index_map[curr_index];

            for (int i = 0; i < 8; i++) {
                if (curr->children[i] == nullptr)
                    continue;
                index_map.push_back(curr->children[i]);
            }

            if (curr->has_data) {
                curr->data_index = data_index;
                data_index++;
            }

            curr->branch_index = curr_index;
        }

        data_size = sizeof(T) * data_index;
        data_array = (T *) malloc(data_size);

        index_size = sizeof(branch_index) * curr_index;
        index_array = (branch_index *) malloc(index_size);

        /* data pass */
        for (curr_index = 0; curr_index < index_map.size(); curr_index++) {
            Node *curr = index_map[curr_index];

            if (curr->has_data)
                data_array[curr->data_index] = curr->data;

            curr_index++;
        }

        //fill_empties(index_map);

        /* index pass */
        for (curr_index = 0, data_index = 0; curr_index < index_map.size(); curr_index++) {
            Node *curr = index_map[curr_index];

            branch_index bi;

            for (int i = 0; i < 8; i++) {
                if (curr->children[i] == nullptr) {
                    bi.branch_index[i] = 0;
                    continue;
                }
                bi.branch_index[i] = curr->children[i]->branch_index;
                if (curr->children[i]->branch_index == -1) {
                    printf("I've lost my child!");
                }
            }

            bi.data_index = curr->data_index;

            if (bi.data_index < 1) {
                printf("Captain we have a problem 252");
            }

            index_array[curr_index] = bi;
            curr_index++;
        }

        levels = max_depth;
    }

    int points() {
        return point_count;
    }

    int splits() {
        return split_count;
    }
};

template<typename T>
oct_tree<T>::oct_tree(vec3 mini, vec3 maxi):  split_count(1), point_count(0), mini(mini), maxi(maxi) {
}



