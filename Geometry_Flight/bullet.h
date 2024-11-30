#pragma once
#include "obj_reader.h"
#include <random>

class Bullet : public Model
{
public:
    float xSpeed;
    float ySpeed;
    float zSpeed;
    bool is_active;

    Bullet() : Model(), xSpeed(0.0f), ySpeed(0.0f), zSpeed(0.0f), is_active(false) {}

    void initialize(const Model& model, float initialSpeed)
    {
        *static_cast<Model*>(this) = model;
        zSpeed = initialSpeed;
        is_active = true;

        colors.resize(vertex_count);
        for (size_t i = 0; i < vertex_count; i++) {
            colors[i] = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-0.005f, 0.005f);
        xSpeed = dis(gen);
        ySpeed = 0.0f;
    }

    void update()
    {
        if (!is_active) return;
        positionX += xSpeed;
        positionY += ySpeed;
        positionZ += zSpeed;
    }

    void deactivate()
    {
        is_active = false;
    }
};