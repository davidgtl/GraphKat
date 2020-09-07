#pragma once

#include <iostream>
#include <map>

#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>
#include "sdf_library/oct_tree.h"


void load_obj(const std::string& inputfile, void *&index, int &index_size,
              void *&data, int &data_size, vec3 &bound_min, vec3 &bound_max, int &levels) {
    using std::map;
    using std::vector;


    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        std::cerr << "could not load object" << std::endl;
        exit(7);
    }

    bool bound_init = false;

    typedef struct {
        vec3 origin;
        vec3 normal;
    } face;

    map<std::pair<int, int>, std::list<face>> edges_face;

    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            assert(fv >= 3);
            vector<vec3> points;
            vector<vec3> normals;
            vector<int> point_index;
            for (size_t v_off = 0; v_off < fv; v_off++) {

                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v_off];
                vec3 v = vec3(
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2]
                ) + vec3(2, 1, 1);

                points.push_back(v);
                point_index.push_back(idx.vertex_index);

                vec3 n = vec3(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                );
                normals.push_back(n);


                if (bound_init) {
                    bound_min = glm::min(bound_min, v);
                    bound_max = glm::max(bound_max, v);
                } else {
                    bound_init = true;
                    bound_min = v;
                    bound_max = v;
                }
            }

            vec3 f_center(0, 0, 0);
            vec3 f_normal(0, 0, 0);

            for (auto &p :points) f_center += p;
            f_center /= fv;

            f_normal = glm::normalize(glm::cross(points[2] - points[0], points[1] - points[0]));

            if (glm::dot(f_normal, normals[0]) < 0)
                f_normal = -f_normal;

            edges_face[{point_index[0], point_index[1]}].push_back({f_center, f_normal});
            edges_face[{point_index[1], point_index[0]}].push_back({f_center, f_normal});
            edges_face[{point_index[1], point_index[2]}].push_back({f_center, f_normal});
            edges_face[{point_index[2], point_index[1]}].push_back({f_center, f_normal});
            edges_face[{point_index[2], point_index[0]}].push_back({f_center, f_normal});
            edges_face[{point_index[0], point_index[2]}].push_back({f_center, f_normal});

            index_offset += fv;
        }
    }

    oct_tree object_tree(bound_min, bound_max);

    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            vector<vec3> points;
            vector<vec3> normals;
            vector<int> point_index;

            for (size_t v_off = 0; v_off < fv; v_off++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v_off];
                vec3 v = vec3(
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2]
                ) + vec3(2, 1, 1);

                points.push_back(v);
                point_index.push_back(idx.vertex_index);

                vec3 n = vec3(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                );
                normals.push_back(n);
                /*
                vec2 t = vec2(
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                );*/
                // Optional: vertex colors
                // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];

            }

            vec3 f_center(0, 0, 0);
            vec3 f_normal(0, 0, 0);

            for (auto &p :points) f_center += p;
            f_center /= fv;

            f_normal = glm::normalize(glm::cross(points[2] - points[0], points[1] - points[0]));

            if (glm::dot(f_normal, normals[0]) < 0)
                f_normal = -f_normal;

            auto vd = new vertex_data{f_center, f_normal};
            std::vector<face> local_face_group;
            for (auto &f: edges_face[{point_index[0], point_index[1]}]) {
                if (f.origin != f_center && f.normal != f_normal)
                    local_face_group.push_back(f);
            }
            for (auto &f: edges_face[{point_index[1], point_index[2]}]) {
                if (f.origin != f_center && f.normal != f_normal)
                    local_face_group.push_back(f);
            }
            for (auto &f: edges_face[{point_index[2], point_index[0]}]) {
                if (f.origin != f_center && f.normal != f_normal)
                    local_face_group.push_back(f);
            }

            if (local_face_group.size() != 3) {
                //printf("Captain we have a local_face_group problem");
                /*while (local_face_group.size() < 3) {
                    local_face_group.push_back({f_center, f_normal});
                }*/
            } else {

                if (points[0] == vec3(0) && points[1] == vec3(0) && points[2] == vec3(0))
                    printf("Captain we have a serious problem");


                object_tree.put(vertex_data{points[0], points[1], points[2],
                                            f_normal,
                                            local_face_group[0].origin, local_face_group[0].normal,
                                            local_face_group[1].origin, local_face_group[1].normal,
                                            local_face_group[2].origin, local_face_group[2].normal}, f_center);
            }
            index_offset += fv;

            // per-face material
            // shapes[s].mesh.nnhhhhh[f];
        }
    }

    object_tree.generate_index_data((oct_tree::branch_index *&) (index), index_size,
                                    (vertex_data *&) data, data_size, levels);


}

void convert_obj(const std::string& inputfile, const std::string& outputfile){
    void *scene_index, *scene_data;
    int scene_index_size, scene_data_size;
    vec3 bound_min, bound_max;
    int levels;
    load_obj(inputfile, scene_index, scene_index_size, scene_data, scene_data_size, bound_min, bound_max,
                levels);

    auto myfile = std::fstream(outputfile, std::ios::out | std::ios::binary);
    myfile.write((char*)&scene_index_size, sizeof(int));
    myfile.write((char*)&scene_data_size, sizeof(int));
    myfile.write((char*)&levels, sizeof(int));
    myfile.write((char*)&bound_min, sizeof(vec3));
    myfile.write((char*)&bound_max, sizeof(vec3));
    myfile.write((char*)scene_index, scene_index_size);
    myfile.write((char*)scene_data, scene_data_size);
    myfile.close();
}

void load_bin(const std::string& inputfile, void *&index, int &index_size,
              void *&data, int &data_size, vec3 &bound_min, vec3 &bound_max, int &levels) {
    auto myfile = std::fstream(inputfile, std::ios::in | std::ios::binary);
    myfile.read((char*)&index_size, sizeof(int));
    myfile.read((char*)&data_size, sizeof(int));
    myfile.read((char*)&levels, sizeof(int));
    myfile.read((char*)&bound_min, sizeof(vec3));
    myfile.read((char*)&bound_max, sizeof(vec3));
    index = malloc(index_size);
    data = malloc(data_size);
    myfile.read((char*)index, index_size);
    myfile.read((char*)data, data_size);
    myfile.close();
}