#pragma once
#include "model.h"
#include <glm/glm.hpp>

#define NORMAL_KEYBOARD_KEYDOWN 0
#define NORMAL_KEYBOARD_KEYUP 1
#define SPECIAL_KEYBOARD_KEYDOWN 2
#define SPECIAL_KEYBOARD_KEYUP 3

class Player : public Model {
public:
    Player() : Model() {}
    float move_x = 0.0f;
    float move_y = 0.0f;
    bool left_pressed = false;
    bool right_pressed = false;

    void init(const Model& model) 
    {
        *static_cast<Model*>(this) = model;
        position_x = 0.0f;
        position_y = 0.0f;
        position_z = 10.0f;
        rotation_x = -90.0f;
        rotation_y = 0.0f;
    }
    void update(float delta_time) 
    {
        if (left_pressed && !right_pressed)
            move_x = -0.01f;
        else if (right_pressed && !left_pressed)
            move_x = 0.01f;
        else
            move_x = 0.0f;        

        this->position_x += move_x;
        this->position_x = clamp_float(-5.0f, this->position_x, 5.0f); // �̵� ���� ����
    }
    void handle_event(int event_type, const char key, int special_key, int x, int y)
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