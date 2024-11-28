#pragma once
#include "obj_reader.h"

class Bullet : public Model 
{
public:
    float xSpeed;
    float ySpeed = 0.0f;
    float zSpeed;  // z�� �̵� �ӵ�

    Bullet(const Model model, float initialSpeed) : Model(model), zSpeed(initialSpeed)
    {
        // ���� �ʱ�ȭ
        colors.resize(vertex_count);
        for (size_t i = 0; i < vertex_count; i++) {
            colors[i] = glm::vec3(1.0f, 1.0f, 1.0f);  // ������� �ʱ�ȭ
        }

        // x���� ���� ���� (ź ���� ����)
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-0.005f, 0.005f);
        xSpeed = dis(gen);
    }

    void update() 
    {
        // �� �����Ӹ��� �̵�
        positionX += xSpeed;
        positionX += ySpeed;
        positionZ += zSpeed;
    }
};

