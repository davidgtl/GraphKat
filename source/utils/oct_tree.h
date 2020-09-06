//
// Created by david on 12/06/2020.
//

#pragma once

#include <glm/glm.hpp>
#include <list>
#include <unordered_map>
#include <vector>

using namespace glm;

struct vertex_data {
    vec3 position_normal[10];
};

class oct_tree {
private:
    vec3 mini, maxi;

    struct Node {
        Node *children[8];
        vertex_data data;
        vec3 value;
        bool has_data = false;
        bool has_ref_data = false;
        int ref_branch_index = -1;
        int branch_index = -1;
        int data_index = -1;
        vec3 mid;

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
        else return find_value(node->children[index], value, child_min, child_max);

    }

    Node *put_value(vertex_data data, Node *node, vec3 value, vec3 mini, vec3 maxi, int levels = 1) {
        if (glm::any(glm::isnan(value)))
            printf("Captain we have a problem 92");
        max_depth = glm::max(max_depth, levels);
        vec3 mid = (mini + maxi) / 2.0f;

        int index = this->index(value, mid);

        vec3 child_min(mini);
        vec3 child_max(maxi);
        child_range(index, mid, child_min, child_max);


        if (glm::any(glm::lessThan(value, child_min)) || glm::any(glm::greaterThan(value, child_max)))
            printf("Captain we have a bounds problem 103");

        bool has_children = false;
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != nullptr)
                has_children = true;
        }

        if (!has_children && !node->has_data && !node->has_ref_data) {
            node->data = data;
            node->value = value;
            node->has_data = true;
            node->mid = mid;
            return node->children[index];
        } else if (!has_children && (node->has_data || node->has_ref_data)) {
            int other_index = this->index(node->value, mid);
            node->children[other_index] = new Node();
            node->children[other_index]->data = node->data;
            node->children[other_index]->data_index = node->data_index;
            node->children[other_index]->has_data = node->has_data;
            node->children[other_index]->has_ref_data = node->has_ref_data;
            node->children[other_index]->value = node->value;

            node->children[index] = new Node();
            node->children[index]->data = data;
            node->children[index]->has_data = true;
            node->children[index]->value = value;
            node->children[index]->mid = (child_min + child_max) * vec3(0.5);

            node->has_data = false;
            node->has_ref_data = false;
            split_count++;
            return node->children[index];
        } // by now it has children for sure
        else if (node->children[index] == nullptr) {
            node->children[index] = new Node();
            node->children[index]->data = data;
            node->children[index]->has_data = true;
            node->children[index]->value = value;
            node->children[index]->mid = (child_min + child_max) * vec3(0.5);
            return node->children[index];
        } else
            return put_value(data, node->children[index], value, child_min, child_max, levels + 1);
    }

    void fill_node(vec3 &value, vertex_data *data, int index) {
        Node *node = put_value(*data, &root, value, mini, maxi);
        node->has_data = false;
        node->has_ref_data = true;
        node->data_index = index;

        if (node->data_index < 0) {
            printf("Captain we have a data_index < 0 problem");
        }
    }

    void fill_empties(const std::vector<Node *> &index_map, vertex_data *data_array, int data_size) {
        float max_dist = 0;

        for (int i = 0; i < index_map.size(); i++) {
            Node *node = index_map[i];

            if (node->has_data || node->has_ref_data) {
                if (node->data_index < 0)
                    printf("Captain we have a problem 146");
                continue;
            }

            float closest_dist = 1000.0f;

            for (int j = 0; j < data_size; j++) {
                vertex_data &data = data_array[j];

                vec3 value =
                        data.position_normal[0] * vec3(0.33) +
                        data.position_normal[1] * vec3(0.33) +
                        data.position_normal[2] * vec3(0.33);


                float new_dist = glm::length(value - node->value);
                if (new_dist < closest_dist) {
                    closest_dist = new_dist;
                    node->data_index = j;
                    node->has_ref_data = true;
                }
            }

            max_dist = glm::max(max_dist, glm::length(node->mid - node->value));
            if (node->data_index < 0)
                printf("Captain we have a problem 162");
        }

        printf("max dist: %f\n", max_dist);
    }

public:
    struct branch_index {
        int data_index;
        int branch_index[8];
    };

    oct_tree(vec3 mini, vec3 maxi) : split_count(1), point_count(0), mini(mini), maxi(maxi) {
    }


    void put(vertex_data data, vec3 value) {
        point_count++;
        put_value(data, &root, value, mini, maxi);
    }

    void find(vec3 value, vertex_data &data, vec3 &actual_value) {
        Node *n = find_value(&root, value, mini, maxi);
        data = n->data;
        actual_value = n->value;
    }

    void generate_index_data(branch_index *&index_array, int &index_size, vertex_data *&data_array, int &data_size,
                             int &levels) {
        using namespace std;

        //allocate index_array and data_array

        std::vector<Node *> index_map;

        index_map.push_back(&root);
        int curr_index = 0;
        int data_index = 0;

        /* node generation along faces */
        for (curr_index = 0; curr_index < index_map.size(); curr_index++) {
            Node *curr = index_map[curr_index];

            for (int i = 0; i < 8; i++) {
                if (curr->children[i] == nullptr)
                    continue;
                index_map.push_back(curr->children[i]);
            }

            if (!curr->has_data) continue;

            curr->data_index = data_index;

            vertex_data *data = &curr->data;
            vec3 value;

            value = data->position_normal[0] * vec3(0.8) + data->position_normal[1] * vec3(0.1) +
                    data->position_normal[2] * vec3(0.1);
            fill_node(value, data, data_index);

            value = data->position_normal[0] * vec3(0.1) + data->position_normal[1] * vec3(0.8) +
                    data->position_normal[2] * vec3(0.1);
            fill_node(value, data, data_index);

            value = data->position_normal[0] * vec3(0.1) + data->position_normal[1] * vec3(0.1) +
                    data->position_normal[2] * vec3(0.8);
            fill_node(value, data, data_index);

            data_index++;
        }

        index_map.clear();
        index_map.push_back(&root);

        // create index
        for (curr_index = 0; curr_index < index_map.size(); curr_index++) {
            Node *curr = index_map[curr_index];

            for (int i = 0; i < 8; i++) {
                if (curr->children[i] == nullptr)
                    continue;
                index_map.push_back(curr->children[i]);
            }

            curr->branch_index = curr_index;
        }

        data_size = sizeof(vertex_data) * data_index;
        data_array = (vertex_data *) malloc(data_size);

        index_size = sizeof(branch_index) * curr_index;
        index_array = (branch_index *) malloc(index_size);

        printf("data_index: %d\n", data_index);
        printf("curr_index: %d\n", curr_index);

        // data pass
        for (curr_index = 0; curr_index < index_map.size(); curr_index++) {
            Node *curr = index_map[curr_index];

            if (curr->has_data) {
                if (curr->data_index < 0) {
                    printf("Captain we have a data_index < 0 problem");
                }
                data_array[curr->data_index] = curr->data;
            }
        }

        fill_empties(index_map, data_array, data_index);

        // index pass
        for (curr_index = 0; curr_index < index_map.size(); curr_index++) {
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
            if (bi.data_index < 0) {
                printf("Captain we have a problem 252");
            }

            index_array[curr_index] = bi;
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