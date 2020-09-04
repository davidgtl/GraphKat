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
        bool has_data;
        bool has_ref_data;
        int ref_index;

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
        max_depth = max(max_depth, levels);
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
                node->children[index]->data = data;
                split_count++;
            } else {
                if (node->has_data) {
                    int other_index = this->index(node->value, mid);
                    node->children[other_index] = new Node();
                    node->children[other_index]->data = node->data;
                    node->children[other_index]->value = node->value;

                    node->children[index] = new Node();
                    node->children[index]->data = data;
                    node->children[index]->value = value;
                    split_count++;

                } else {
                    node->data = data;
                    node->value = value;
                    node->has_data = true;
                }
            }
            return;
        } else put_value(data, node->children[index], value, child_min, child_max, levels + 1);
    }

    Node* fill_empties(vec3 bound_min = vec3(0,0,0), vec3 bound_max = vec3(0,0,0), Node * node = nullptr){
        if(node == nullptr) {
            node = &root;
            bound_min = mini;
            bound_max = maxi;
        }

        vec3 mid = (bound_min + bound_max) * 0.5f;

        Node * best_found_node = nullptr;
        for(int index = 0; index < 8; index++) {
            vec3 child_min(bound_min);
            vec3 child_max(bound_max);
            child_range(index, mid, child_min, child_max);

            if (node->children[index] != nullptr) {
                Node* found_node = fill_empties(child_min, child_max, node->children[index]);

                if(best_found_node == nullptr || glm::length(mid - found_node->value) < glm::length(mid - best_found_node->value))
                    best_found_node = found_node;
            }
        }

        return best_found_node;
    }

public:
    struct branch_index {
        vec3 midpoint;
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
    void generate_index_data(branch_index *&index_array, int &index_size, T *&data_array, int &data_size, int& levels) {
        using namespace std;

        //allocate index_array and data_array

        std::vector<Node*> index_map;

        index_map.push_back(&root);
        int curr_index = 0;
        int data_index = 0;

        // create index hashmap
        for(curr_index = 0; curr_index < index_map.size(); curr_index++){
            Node *curr = index_map[curr_index];

            for (int i = 0; i < 8; i++) {
                if (curr->children[i] == nullptr)
                    continue;
                index_map.push_back(curr->children[i]);
            }
            if(curr->has_data)
                data_index++;
        }

        data_size = data_index;
        data_array = (T*)malloc(sizeof(T) * data_size);

        index_size = curr_index;
        index_array = (branch_index *)malloc(sizeof(branch_index) * index_size);

        fill_empties();

        for(curr_index = 0, data_index = 0; curr_index < index_map.size(); curr_index++){
            Node *curr = index_map[curr_index];

            branch_index bi;

            for (int i = 0; i < 8; i++) {
                if (curr->children[i] == nullptr) {
                    bi.branch_index[i] = 0;
                    continue;
                }
                bool found_child = false;
                for(int child_index = curr_index; child_index < index_map.size(); child_index++)
                    if(index_map[child_index] == curr->children[i]) {
                        bi.branch_index[i] = child_index; //FIXME: set me as field in Node * and use me in fill_empties
                        found_child = true;
                        break;
                    }
                if(!found_child){
                    printf("I've lost my child!");
                }
            }

            if (curr->has_data) {
                bi.data_index = data_index; // think about overshooting due to large empty spaces
                data_array[data_index++] = curr->data;
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



