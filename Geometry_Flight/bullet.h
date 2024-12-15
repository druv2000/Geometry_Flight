#pragma once
#include "object.h"
#include "collision_manager.h"
#include <random>

class Bullet : public Object
{
public:
    Bullet() : Object() {}
    int max_hp = 1;
    int cur_hp = 1;
    int attack_damage = 1;

    float x_speed = 0.0f;
    float y_speed = 0.0f;
    float z_speed = 0.0f;
    bool is_active = false;

    void init(const Model& model, float x, float y, float z, float initial_speed)
    {
        Object::init(model, x, y, z);
        is_active = true;
        type = TYPE_BULLET_1;
        z_speed = initial_speed;

        colors.resize(vertex_count);
        for (size_t i = 0; i < vertex_count; i++) {
            colors[i] = glm::vec3(1.0f, 1.0f, 0.0f);
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-0.001f, 0.001f);
        x_speed = dis(gen);
        y_speed = 0.0f;

        for (size_t i = 0; i < vertex_count; i++)
        {
            texcoord.push_back(glm::vec2((vertices[i].x * vertices[i].z + 1) / 2.0f, (vertices[i].y * vertices[i].z + 1) / 2.0f));
        }
    }
    void update(float delta_time) override
    {
        Object::update(delta_time);

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
        bb.top_left_front.x = position_x + 0.2f;
        bb.top_left_front.y = position_y + 0.2f;
        bb.top_left_front.z = position_z + 0.2f;

        bb.bottom_right_back.x = position_x - 0.2f;
        bb.bottom_right_back.y = position_y - 0.2f;
        bb.bottom_right_back.z = position_z - 0.2f;

        return bb;
    }
    void deactivate()
    {
        // 각종 초기화. 
        is_active = false;
        remove_collision_object(this);
    }
    void handle_collision(std::string group, Object* other) override
    {
        if (group == "ally_bullet:enemy")
        {
            this->deactivate();
        }
    }
};