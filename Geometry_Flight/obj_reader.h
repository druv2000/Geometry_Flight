#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>


#define MAX_LINE_LENGTH 1000

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    unsigned int v1, v2, v3;
} Face;

typedef struct {
    Vertex* vertices;
    size_t vertex_count;
    Face* faces;
    size_t face_count;

    int modelType;

    std::vector<glm::vec3> colors;  // 각 정점의 색상을 저장

    float rotationX;
    float rotationY;
    float rotationZ;

    float positionX;
    float positionY;
    float positionZ;

    float faceRotationX;  // 면의 X축 회전
    float faceRotationY;  // 면의 Y축 회전
    float faceRotationZ;  // 면의 Z축 회전
} Model;

void read_newline(char* str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}

void read_obj_file(const char* filename, Model* model) 
{
    FILE* file;

    //std::cout << "Attempting to open file: " << filename << std::endl;
    fopen_s(&file, filename, "r");
    if (!file) 
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    //std::cout << "File opened successfully" << std::endl;

    fopen_s(&file, filename, "r");
    if (!file) 
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    model->vertex_count = 0;
    model->face_count = 0;
    while (fgets(line, sizeof(line), file)) 
    {
        read_newline(line);
        if (line[0] == 'v' && line[1] == ' ')
            model->vertex_count++;
        else if (line[0] == 'f' && line[1] == ' ')
            model->face_count++;
    }
    fseek(file, 0, SEEK_SET);
    model->vertices = (Vertex*)malloc(model->vertex_count * sizeof(Vertex));
    model->faces = (Face*)malloc(model->face_count * sizeof(Face));
    size_t vertex_index = 0;
    size_t face_index = 0;
    while (fgets(line, sizeof(line), file))
    {
        read_newline(line);
        if (line[0] == 'v' && line[1] == ' ') 
        {
            sscanf_s(line + 2, "%f %f %f", &model->vertices[vertex_index].x,
                &model->vertices[vertex_index].y,
                &model->vertices[vertex_index].z);
            vertex_index++;
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            unsigned int v1, v2, v3, v4;
            int count = sscanf_s(line + 2, "%u//%*u %u//%*u %u//%*u %u//%*u", &v1, &v2, &v3, &v4);

            model->faces[face_index].v1 = v1;
            model->faces[face_index].v2 = v2;
            model->faces[face_index].v3 = v3;
            face_index++;
        }
    }
    fclose(file);
}