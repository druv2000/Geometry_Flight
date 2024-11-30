#pragma once
#include "bullet.h"

class BulletPool
{
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
                bullet.initialize(model, initialSpeed);
                return &bullet;
            }
        }
        return nullptr; // 사용 가능한 총알이 없을 경우
    }

    void updateBullets()
    {
        for (auto& bullet : bullets)
        {
            if (bullet.is_active)
            {
                bullet.update();
                if (bullet.positionZ <= -100.0f)
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