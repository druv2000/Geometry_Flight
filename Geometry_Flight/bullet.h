#pragma once
#include "obj_reader.h"

class Bullet : public Model 
{
public:
    float xSpeed;
    float ySpeed = 0.0f;
    float zSpeed;  // z축 이동 속도

    Bullet(const Model model, float initialSpeed) : Model(model), zSpeed(initialSpeed)
    {
        // 색상 초기화
        colors.resize(vertex_count);
        for (size_t i = 0; i < vertex_count; i++) {
            colors[i] = glm::vec3(1.0f, 1.0f, 1.0f);  // 흰색으로 초기화
        }

        // x방향 랜덤 설정 (탄 퍼짐 현상)
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-0.005f, 0.005f);
        xSpeed = dis(gen);
    }

    void update() 
    {
        // 매 프레임마다 이동
        positionX += xSpeed;
        positionX += ySpeed;
        positionZ += zSpeed;
    }
};

