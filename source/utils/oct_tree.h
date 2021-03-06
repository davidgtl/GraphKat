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
    vec3 position_normal[7];
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

    void put_value(vertex_data data, Node *node, vec3 value, vec3 mini, vec3 maxi, int levels = 1) {
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

        if (!has_children && !node->has_data) {
            node->data = data;
            node->value = value;
            node->has_data = true;
            node->mid = mid;
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
            node->children[index]->mid = (child_min + child_max) * vec3(0.5);

            node->has_data = false;
            split_count++;
        } // by now it has children for sure
        else if (node->children[index] == nullptr) {
            node->children[index] = new Node();
            node->children[index]->data = data;
            node->children[index]->has_data = true;
            node->children[index]->value = value;
            node->children[index]->mid = (child_min + child_max) * vec3(0.5);
        } else
            put_value(data, node->children[index], value, child_min, child_max, levels + 1);
    }

    float smin(float x, float y, float smoothness) {
        float h = glm::max(smoothness - glm::abs(x - y), 0.0f) / smoothness;
        return glm::min(x, y) - h * h * h * smoothness * (1.0f / 6.0f);
    }

    glm::vec3 obj_data_origin(vertex_data &d, int face){
        return d.position_normal[face * 2 + 0];
    }

    glm::vec3 obj_data_normal(vertex_data &d, int face){
        return d.position_normal[face * 2 + 1];
    }

    float sdInfiniplane(vec3 p, vec3 o, vec3 n)
    {
        vec3 dv = p - o;
        float dist = dot(dv, n);
        if(dist < 0) return 1000.0f;
        return dist;
    }

    float calc_dist(vec3 &p, vertex_data &d) {
        float minDist  = 1000.0f;
        float k = 0.01;

        vec3 p0 = d.position_normal[1];
        vec3 p1 = d.position_normal[2];
        vec3 p2 = d.position_normal[3];
        vec3 dp = p - p0;
        vec3 ax1 = p1 - p0;
        vec3 ax2 = p2 - p0;

        float planeDist = dot(dp, d.position_normal[0]);

        minDist = planeDist;

        vec2 coords = vec2(glm::dot(dp, ax1) / glm::pow(glm::length(ax1), 2),
                           glm::dot(dp, ax2) / glm::pow(glm::length(ax2), 2));

        if(any(lessThan(coords, vec2(0))) || any(greaterThan(coords, vec2(1))))
            minDist = length(p0 + (ax1 + ax2) * vec3(0.5));

        return minDist;
    }

    void fill_empties(const std::vector<Node *> &index_map) {
        float max_dist = 0;
        for (int i = 0; i < index_map.size(); ++i) {
            Node *node = index_map[i];

            if (node->has_data) {
                if (node->data_index < 1)
                    printf("Captain we have a problem 146");
                continue;
            }

            float closest_dist = 1000.0f;

            for (int j = 0; j < index_map.size(); ++j) {
                Node *other = index_map[j];
                if (!other->has_data)
                    continue;
                float new_dist = calc_dist(node->value, other->data);
                if (new_dist < closest_dist) {
                    closest_dist = new_dist;
                    node->data_index = other->data_index;
                    node->has_ref_data = true;
                }
            }
            max_dist = glm::max(max_dist, closest_dist);
            if (node->data_index < 1)
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

        data_size = sizeof(vertex_data) * data_index;
        data_array = (vertex_data *) malloc(data_size);

        index_size = sizeof(branch_index) * curr_index;
        index_array = (branch_index *) malloc(index_size);

        printf("data_index: %d\n", data_index);
        printf("curr_index: %d\n", curr_index);

        /* data pass */
        for (curr_index = 0; curr_index < index_map.size(); curr_index++) {
            Node *curr = index_map[curr_index];

            if (curr->has_data) {
                data_array[curr->data_index] = curr->data;
                //data_array[curr->data_index].cookie = curr->data_index;
            }
        }

        fill_empties(index_map);

        /* index pass */
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
            if (bi.data_index < 1) {
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