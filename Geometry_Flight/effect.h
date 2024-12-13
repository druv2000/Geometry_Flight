#pragma once
#include <string>
#include <chrono>

class Object; // ���� ����

// ������Ʈ�� �ο��Ǵ� ���� ����Ʈ���� �����ϴ� Ŭ����.
// ȿ���� ����� ��, ���ӽð� ���߿�, ����� �� ������ �۾����� ������ �� ����
// �̰� ��ӹ޴� ���ο� Ŭ������ ���� ����Ʈ�� �����ϸ� ��.
class Effect
{
public:
    Effect(const std::string& name, float duration)
        : name(name), duration(duration), start_time(std::chrono::steady_clock::now()), is_active(true) {}

    virtual bool isActive() const
    {
        auto current_time = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / 1000.0f < duration;
    }

    virtual void apply(Object* character) = 0;      // ȿ���� ����� �� ����� �۾���
    virtual void remove(Object* character) = 0;     // ȿ���� ����� �� ����� �۾���
    virtual void update(Object* character)          // ���ӽð� ���߿� ����� �۾���
    {
        if (!isActive()) 
        {
            is_active = false;
        }
    }

    void refresh() 
    {
        start_time = std::chrono::steady_clock::now();
        is_active = true;
    }

protected:
    std::string name;
    float duration;
    std::chrono::steady_clock::time_point start_time;
    bool is_active;
};