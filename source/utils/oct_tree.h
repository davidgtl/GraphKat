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

        Node() {
            for (int i = 0; i < 8; i++) children[i] = nullptr;
            has_data = false;
        }
    };

    typedef struct FUCPP{
        Node * pointer;
        FUCPP(){
            pointer = nullptr;
        }

        FUCPP(Node * ptr_val){
            pointer = ptr_val;
        }

        ~FUCPP(){}
    } FUCPP;

    int point_count;
    int split_count;
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

    void put_value(T data, Node *node, vec3 value, vec3 mini, vec3 maxi) {
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
        } else put_value(data, node->children[index], value, child_min, child_max);
    }

    void fill_empties(){

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
    void generate_index_data(branch_index *&index_array, int &index_size, T *&data_array, int &data_size) {
        using namespace std;

        //allocate index_array and data_array

        fill_empties();

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
                        bi.branch_index[i] = child_index;
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


        printf("end.");
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



