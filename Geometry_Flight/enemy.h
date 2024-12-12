#pragma once
#include "object.h"

class Enemy : public Object {
public:
    Enemy() : Object() {}

    void init(const Model& model, float x, float y, float z) override
    {
        Object::init(model, x, y, z);
    }
    void update(float delta_time) override
    {
        this->position_z += 0.05f;
        this->position_z = clamp_float(-50.0f, this->position_z, 50.0f); // 이동 범위 제한

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
};