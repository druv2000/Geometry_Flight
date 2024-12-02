#pragma once
#include "bullet.h"

class BulletPool {
private:
    std::vector<Bullet> bullets;
    size_t poolSize;

public:
    BulletPool(size_t size) : poolSize(size)
    {
        bullets.resize(size);
    }

    Bullet* getBullet(const Model& model, float initialSpeed)
    {
        for (auto& bullet : bullets)
        {
            if (!bullet.is_active)
            {
                bullet.init(model, initialSpeed);
                return &bullet;
            }
        }
        return nullptr; // ��� ������ �Ѿ��� ���� ���
    }
    void update(float delta_time)
    {
        for (auto& bullet : bullets)
        {
            if (bullet.is_active)
            {
                bullet.update(delta_time);
                if (bullet.position_z <= -500.0f)
                {
                    bullet.deactivate();
                }
            }
        }
    }
    std::vector<Bullet>& getAllBullets()
    {
        return bullets;
    }
};