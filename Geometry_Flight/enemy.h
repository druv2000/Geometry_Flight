#pragma once
#include "object.h"
#include "collision_manager.h"


class Enemy : public Object {
public:
    Enemy() : Object() {}

    void init(const Model& model, float x, float y, float z) override
    {
        Object::init(model, x, y, z);
        type = TYPE_ENEMY_1;
        max_hp = 5;
        cur_hp = 5;
        attack_damage = 1;
        colors.resize(vertex_count);
        for (size_t i = 0; i < vertex_count; i++)
        {
            colors[i] = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        for (size_t i = 0; i < vertex_count; i++)
        {
            texcoord.push_back(glm::vec2((vertices[i].x * vertices[i].z + 1) / 2.0f, (vertices[i].y * vertices[i].z + 1) / 2.0f));
        }
    }
    void update(float delta_time) override
    {
        Object::update(delta_time);

        this->position_z += 0.025f * delta_time * 1000;
        this->position_z = clamp_float(-50.0f, this->position_z, 50.0f); // �̵� ���� ����

        if (this->position_z > 15.0f)
        {
            this->is_active = false;
        }
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
    void handle_collision(std::string group, Object* other) override
    {
        if (group == "ally_bullet:enemy")
        {
            // �÷��̾� �Ѿ˰� �浹���� ���
            this->take_damage(other->attack_damage);
        }

        else if (group == "player:enemy")
        {
            // �÷��̾�� �浹���� ���
            // �� ���µ�? ó���Ұ� �ֳ�
        }
    }
};