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

        /*position_x = 0.0f;
        position_y = 0.0f;
        position_z = 10.0f;*/

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
        this->position_x = clamp_float(-5.0f, this->position_x, 5.0f); // �̵� ���� ����
    }
    void handle_event(int event_type, const char key, int special_key, int x, int y) override
    {
        if (event_type == NORMAL_KEYBOARD_KEYDOWN)
        {
            // �Ϲ� Ű keydown
        }
        else if (event_type == NORMAL_KEYBOARD_KEYUP)
        {
            // �Ϲ� Ű keyup
        }
        if (event_type == SPECIAL_KEYBOARD_KEYDOWN)
        {
            // Ư�� Ű keydown
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
            // Ư�� Ű keyup
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
};