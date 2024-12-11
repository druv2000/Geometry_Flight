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
    void handle_event(int event_type, const char key, int special_key, int x, int y) override
    {
        
    }
};