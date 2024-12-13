#pragma once
#include "object.h"

#define NORMAL_KEYBOARD_KEYDOWN 0
#define NORMAL_KEYBOARD_KEYUP 1
#define SPECIAL_KEYBOARD_KEYDOWN 2
#define SPECIAL_KEYBOARD_KEYUP 3

class Player : public Object {
public:
    Player() : Object() {}
    float move_x = 0.0f;
    float move_y = 0.0f;
    bool left_pressed = false;
    bool right_pressed = false;

    void init(const Model& model, float x, float y, float z) override
    {
        Object::init(model, x, y, z);
        type = TYPE_PLAYER;
        rotation_x = -90.0f;
        rotation_y = 0.0f;

        /*colors.resize(vertex_count);
        for (size_t i = 0; i < vertex_count - 1; i++)
        {
            colors[i] = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        colors[vertex_count - 1] = glm::vec3(0.0f, 1.0f, 0.0f);*/
    }
    void update(float delta_time) override
    {
        if (left_pressed && !right_pressed)
            move_x = -0.02f;
        else if (right_pressed && !left_pressed)
            move_x = 0.02f;
        else
            move_x = 0.0f;        

        this->position_x += move_x;
        this->position_x = clamp_float(-5.0f, this->position_x, 5.0f); // 이동 범위 제한
    }
    void handle_events(int event_type, const char key, int special_key, int x, int y) override
    {
        if (event_type == NORMAL_KEYBOARD_KEYDOWN)
        {
            // 일반 키 keydown
        }
        else if (event_type == NORMAL_KEYBOARD_KEYUP)
        {
            // 일반 키 keyup
        }
        if (event_type == SPECIAL_KEYBOARD_KEYDOWN)
        {
            // 특수 키 keydown
            switch (special_key) {
            case GLUT_KEY_LEFT:
                left_pressed = true;
                break;
            case GLUT_KEY_RIGHT:
                right_pressed = true;
                break;
            }
        }
        else if (event_type == SPECIAL_KEYBOARD_KEYUP)
        {
            // 특수 키 keyup
            switch (special_key) {
            case GLUT_KEY_LEFT:
                left_pressed = false;
                break;
            case GLUT_KEY_RIGHT:
                right_pressed = false;
                break;
            }
        }
        else
        {
            
        }
    }
    BB get_bb() override
    {
        BB bb;
        bb.top_left_front.x = position_x + 0.5f;
        bb.top_left_front.y = position_y + 0.5f;
        bb.top_left_front.z = position_z + 0.5f;

        bb.bottom_right_back.x = position_x - 0.5f;
        bb.bottom_right_back.y = position_y - 0.5f;
        bb.bottom_right_back.z = position_z - 0.5f;
        
        return bb;
    }
};