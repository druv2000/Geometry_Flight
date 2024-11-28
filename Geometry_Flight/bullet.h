#pragma once
#include "obj_reader.h"

class Bullet : public Model 
{
public:
    // 각 축 이동속도
    float xSpeed;
    float ySpeed = 0.0f;
    float zSpeed; 

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

//플레이어 클래스
class Player : public Model
{
public:
    // 각 축 이동속도
    float xSpeed = 0.0f;
    float ySpeed = 0.0f;
    float zSpeed = 0.0f;
    //시작 생명 3개
    int life = 3;
    //
    Player(const Model model) : Model(model)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.005f, 0.995f);
        // 색상 초기화
        colors.resize(vertex_count);
        for (size_t i = 0; i < vertex_count; i++) {
            colors[i] = glm::vec3(dis(gen), dis(gen), dis(gen));  // 흰색으로 초기화
        }

        //위아래로 흔들림
        //fill
    }

    void update()
    {
        // 매 프레임마다 이동
        positionX += xSpeed;
        positionX += ySpeed;
        positionZ += zSpeed;
    }
};

