#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include "model.h"

#define MAX_LINE_LENGTH 1000

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
    char line[MAX_LINE_LENGTH];
    model->vertex_count = 0;
    model->face_count = 0;
    model->normal_count = 0;
    while (fgets(line, sizeof(line), file)) 
    {
        read_newline(line);
        if (line[0] == 'v' && line[1] == ' ')
            model->vertex_count++;
        else if (line[0] == 'f' && line[1] == ' ')
            model->face_count++; 
        else if (line[0] == 'v' && line[1] == 'n')
            model->normal_count++;
    }
    fseek(file, 0, SEEK_SET);
    model->vertices = (Vertex*)malloc(model->vertex_count * sizeof(Vertex));
    model->faces = (Face*)malloc(model->face_count * sizeof(Face));

    size_t vertex_index = 0;
    size_t face_index = 0;
    size_t normal_index = 0;
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
            unsigned int v1, v2, v3, n1, n2, n3;
            int count = sscanf_s(line + 2, "%u//%u %u//%u %u//%u", &v1, &n1, &v2, &n2, &v3, &n3);

            model->faces[face_index].v1 = v1;
            model->faces[face_index].v2 = v2;
            model->faces[face_index].v3 = v3;
            model->faces[face_index].n1 = n1;
            model->faces[face_index].n2 = n2;
            model->faces[face_index].n3 = n3;
            face_index++;
        }
        else if (line[0] == 'v' && line[1] == 'n')
        {

            glm::vec3 normal;
            int count = sscanf_s(line + 2, "%f %f %f", &normal.x, &normal.y, &normal.z);
            model->normals.push_back(normal);
            normal_index++;
        }
    }
    fclose(file);
}