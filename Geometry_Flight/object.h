#pragma once
#pragma once
#include "model.h"
#include "effect.h"
#include <glm/glm.hpp>

#define TYPE_DEFAULT 0
#define TYPE_PLAYER 1
#define TYPE_BULLET_1 2
#define TYPE_BULLET_2 3
#define TYPE_ENEMY_1 4
#define TYPE_ENEMY_2 5

class Object : public Model {
public:
    Object() : Model() {}
    bool is_active = true;
    int max_hp = 1;
    int cur_hp = 1;
    int attack_damage = 1;
    int hit_count = 0;

    virtual void init(const Model& model, float x, float y, float z) 
    {
        *static_cast<Model*>(this) = model;
        position_x = x;
        position_y = y;
        position_z = z;
        type = TYPE_DEFAULT;
    }
    virtual void update(float delta_time) 
    {
        // ���� Ȱ��ȭ�� ����Ʈ ����. ���ӽð��� ���� ����Ʈ�� ������
        std::vector<std::shared_ptr<Effect>> active_effects;
        for (auto& effect : effects) {
            if (effect->isActive()) {
                effect->update(this);
                active_effects.push_back(effect);
            }
            else {
                effect->remove(this);
            }
        }
        effects = active_effects;
    }
    virtual void handle_events(int event_type, const char key, int special_key, int x, int y) {}
    virtual BB get_bb() 
    {
        // ��Ʈ�ڽ� ���ϱ�. �� ������ ������ü�� ������.(ȸ�� ����)
        // �̰� �⺻���̰�, �� ������Ʈ Ŭ�������� �������̵� �� ����

        BB bb;
        bb.top_left_front.x = 0.0f;
        bb.top_left_front.y = 0.0f;
        bb.top_left_front.z = 0.0f;

        bb.bottom_right_back.x = 0.0f;
        bb.bottom_right_back.y = 0.0f;
        bb.bottom_right_back.z = 0.0f;

        return bb;
    }
    virtual void handle_collision(std::string group, Object* other)
    {
        // �浹 �� ó�� �Լ�. ���� ������Ʈ Ŭ�������� �������̵� �� ����
    }
    virtual void take_damage(int amount)
    {
        // ������ ó�� �Լ�.
        this->cur_hp -= amount;
        clamp_int(0, this->cur_hp, this->max_hp);

        // �ǰ� ����Ʈ (�� �ٲٱ�)
        colors.resize(vertex_count);
        for (size_t i = 0; i < vertex_count; i++)
        {
            if (this->hit_count % 2 == 1)
                colors[i] = glm::vec3(1.0f, 1.0f, 1.0f);
            else
                colors[i] = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        
        // hp�� 0�� �Ǹ�
        if (cur_hp <= 0)
        {
            this->is_active = false;  // ���� ��ü�� ��Ȱ��ȭ. �浹 �׷� ������ �� ���������� update()���� �ؾ� ��.
        }

        this->hit_count++;
    }
    void addEffect(std::shared_ptr<Effect> effect)
    {
        effect->apply(this);
        effects.push_back(effect);
    }
    void removeEffect(const std::type_info& effect_type)
    {
        effects.erase(
            std::remove_if(effects.begin(), effects.end(),
                [&](const std::shared_ptr<Effect>& effect) 
                {
                    if (typeid(*effect) == effect_type) 
                    {
                        effect->remove(this);
                        return true;
                    }
                    return false;
                }
            ),
            effects.end()
        );
    }

private:
    std::vector<std::shared_ptr<Effect>> effects;

protected:
    // ����ü �� clamp�� ������, �Ǽ��� ���� ������ ��?
    float clamp_float(float min, float value, float max)
    {
        return std::max(min, std::min(value, max));
    }
    int clamp_int(int min, int value, int max)
    {
        return std::max(min, std::min(value, max));
    }
};