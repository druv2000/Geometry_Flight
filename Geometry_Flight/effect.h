#pragma once
#include <string>
#include <chrono>

class Object; // 전방 선언

// 오브젝트에 부여되는 각종 이펙트들을 관리하는 클래스.
// 효과가 적용될 때, 지속시간 도중에, 종료될 때 수행할 작업들을 지정할 수 있음
// 이걸 상속받는 새로운 클래스로 각종 이펙트를 구현하면 됨.
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

    virtual void apply(Object* character) = 0;      // 효과가 적용될 때 수행될 작업들
    virtual void remove(Object* character) = 0;     // 효과가 종료될 때 수행될 작업들
    virtual void update(Object* character)          // 지속시간 도중에 수행될 작업들
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