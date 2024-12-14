#pragma once
#include "model.h"
#include <glm/glm.hpp>
#include <vector>

#define NORMAL_KEYBOARD_KEYDOWN 0
#define NORMAL_KEYBOARD_KEYUP 1
#define SPECIAL_KEYBOARD_KEYDOWN 2
#define SPECIAL_KEYBOARD_KEYUP 3

class Background : public Model {
public:
    Background() : Model() {}
    float rotation_speed = 0.1f;
    std::vector<Model> cube_models;

    void init(const Model& model, const Model& cube)
    {
        *static_cast<Model*>(this) = model;
        position_x = 0.0f;
        position_y = -40.0f;
        position_z = -20.0f;

        rotation_x = 0.0f;
        rotation_y = 0.0f;
        rotation_z = 90.0f;

        for (size_t i = 0; i < vertex_count; i++)
        {
            texcoord.push_back(glm::vec2((vertices[i].x * vertices[i].z + 1) / 2.0f, (vertices[i].y * vertices[i].z + 1) / 2.0f));
        }
        generate_random_cubes(100, cube); // 50개의 큐브 생성
    }

    void update(float delta_time)
    {
        this->rotation_x += rotation_speed * delta_time * 1000; // 1000fps 기준
    }

    void handle_event(int event_type, const char key, int special_key, int x, int y)
    {
        // 이벤트 처리 로직
    }

    void generate_random_cubes(int count, const Model& cube_template)
    {
        cube_models.clear();
        for (int i = 0; i < count; ++i)
        {   
            // 원통을 수직으로 눕혔으므로 height, radius 또한 누운 상태를 가정하고 적용해야 함
            float theta = glm::radians(glm::linearRand(0.0f, 360.0f));
            float height;                                   // 건물 배치가 가능한 너비 (-1 ~ 1: 원통의 양 끝 범위)
            float radius = glm::linearRand(0.75f, 1.4f);    // 건물 배치가 가능한 높이 (1.5: 표면에 생성)

            // 중앙을 비우는 로직
            if (glm::linearRand(0.0f, 1.0f) < 0.5f)
            {
                height = glm::linearRand(0.4f, 1.0f);
            }
            else
            {
                height = glm::linearRand(-1.0f, -0.4f);
            }

            glm::vec3 position(
                radius * cos(theta),
                height,
                radius * sin(theta)
            );

            Model cube = cube_template;
            cube.position_x = position.x;
            cube.position_y = position.y;
            cube.position_z = position.z;

            // 실린더 중심을 향하는 회전 계산
            float rotation_y = glm::degrees(atan2(position.x, position.z));
            cube.rotation_y = rotation_y;
            cube.rotation_x = 0.0f;
            cube.rotation_z = 0.0f;

            //// 랜덤 스케일 추가
            //float scale = glm::linearRand(0.05f, 0.15f);
            //cube.scale_x = scale;
            //cube.scale_y = scale;
            //cube.scale_z = scale;

            //텍스쳐 좌표 추가
            for (size_t i = 0; i < cube.vertex_count; i++)
            {
                cube.texcoord.push_back(glm::vec2((cube.vertices[i].x * cube.vertices[i].z + 1) / 2.0f, (cube.vertices[i].y * cube.vertices[i].z + 1) / 2.0f));
            }
            cube_models.push_back(cube);
        }
    }
};