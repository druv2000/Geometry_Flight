#pragma once
#pragma once
#include "model.h"
#include <glm/glm.hpp>

class Object : public Model {
public:
    Object() : Model() {}
    bool is_active = true;

    virtual void init(const Model& model, float x, float y, float z) 
    {
        *static_cast<Model*>(this) = model;
        position_x = x;
        position_y = y;
        position_z = z;
    }
    virtual void update(float delta_time) {}
    virtual void handle_event(int event_type, const char key, int special_key, int x, int y) {}

protected:
    float clamp_float(float min, float value, float max)
    {
        return std::max(min, std::min(value, max));
    }
};