#pragma once
#include "obj_reader.h"
#include <random>

class Bullet : public Model
{
public:
    float x_speed;
    float y_speed;
    float z_speed;
    bool is_active;

    Bullet() : Model(), x_speed(0.0f), y_speed(0.0f), z_speed(0.0f), is_active(false) {}

    void init(const Model& model, float initialSpeed)
    {
        *static_cast<Model*>(this) = model;
        z_speed = initialSpeed;
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
    void update(float delta_time)
    {
        if (!is_active) return;
        position_x += x_speed * delta_time * 1000.0f; // 1000fps 기준으로 정규화
        position_y += y_speed * delta_time * 1000.0f;
        position_z += z_speed * delta_time * 1000.0f;
    }
    void handle_events(float frame_time)
    {

    }
    void deactivate()
    {
        is_active = false;
    }
};