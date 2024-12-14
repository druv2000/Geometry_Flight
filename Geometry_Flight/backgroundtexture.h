#pragma once
#pragma once
#include "model.h"
#include <glm/glm.hpp>
#include <vector>


class BackgroundTexture : public Model {
public:
    BackgroundTexture() : Model() {}
    float rotation_speed = 0.01f;

    void init(const Model& model)
    {
        *static_cast<Model*>(this) = model;
        position_x = 0.0f;
        position_y = 10.0f;
        position_z = 20.0f;

        rotation_x = -26.57f;
        rotation_y = 0.0f;
        rotation_z = 0.0f;

        colors.clear();
        texcoord.clear();
        for (size_t i = 0; i < vertex_count; i++) {
            colors.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
            texcoord.push_back(glm::vec2((vertices[i].x * vertices[i].z + 1) / 2.0f, (vertices[i].y * vertices[i].z + 1) / 2.0f));
        }
    }
    void update(float delta_time)
    {
        //업데이트
    }

    void handle_event(int event_type, const char key, int special_key, int x, int y)
    {
        // 이벤트 처리 로직
    }

};