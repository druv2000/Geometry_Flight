#pragma once
#include "effect.h"
#include <glm/glm.hpp>

class Object;

class HitEffect : public Effect {
public:
    HitEffect(float duration = 0.2f) : Effect("HitEffect", duration), original_colors() {}

    void apply(Object* character) override
    {
        original_colors = character->colors;
        //for (auto& color : character->colors)
        //{
        //    color = glm::vec3(1.0f, 1.0f, 1.0f); // 흰색으로 변경
        //}
    }

    void remove(Object* character) override 
    {
        character->colors = original_colors;
    }

    void update(Object* character) override 
    {
        Effect::update(character);
        if (!isActive()) {
            remove(character);
        }
    }

private:
    std::vector<glm::vec3> original_colors;
};