// model.h
#pragma once


#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <chrono>
#include <thread>
#include <cmath>
#include <vector>
#include <unordered_map>

#include "utility.h"


typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    unsigned int v1, v2, v3;
    unsigned int n1, n2, n3;
} Face;

typedef struct {
    Vertex top_left_front;
    Vertex bottom_right_back;
}BB;

typedef struct {
    Vertex* vertices;
    size_t vertex_count;
    std::vector<glm::vec2> texcoord;
    Face* faces;
    size_t face_count;
    std::vector<glm::vec3> normals;
    size_t normal_count;
    int type;

    std::vector<glm::vec3> colors;
    float rotation_x;
    float rotation_y;
    float rotation_z;
    float position_x;
    float position_y;
    float position_z;
    BB bb;

} Model;