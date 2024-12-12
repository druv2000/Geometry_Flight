#pragma once
#include "object.h"
#include <random>

class Bullet : public Object
{
public:
    Bullet() : Object() {}

    float x_speed = 0.0f;
    float y_speed = 0.0f;
    float z_speed = 0.0f;
    bool is_active = false;


    void init(const Model& model, float x, float y, float z, float initial_speed)
    {
        Object::init(model, x, y, z);

        z_speed = initial_speed;
        is_active = true;

        colors.resize(vertex_count);
        for (size_t i = 0; i < vertex_count; i++) {
            colors[i] = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-0.005f, 0.005f);
        x_speed = dis(gen);
        y_speed = 0.0f;
    }
    void update(float delta_time) override
    {
        if (!is_active) return;
        position_x += x_speed * delta_time * 1000.0f; // 1000fps 기준으로 정규화
        position_y += y_speed * delta_time * 1000.0f;
        position_z += z_speed * delta_time * 1000.0f;
    }
    void handle_events(int event_type, const char key, int special_key, int x, int y) override
    {

    }
    BB get_bb() override
    {
        BB bb;
        bb.top_left_front.x = position_x + 1.0f;
        bb.top_left_front.y = position_y + 1.0f;
        bb.top_left_front.z = position_z + 1.0f;

        bb.bottom_right_back.x = position_x - 1.0f;
        bb.bottom_right_back.y = position_y - 1.0f;
        bb.bottom_right_back.z = position_z - 1.0f;

        return bb;
    }
    void deactivate()
    {
        is_active = false;
    }
    
};