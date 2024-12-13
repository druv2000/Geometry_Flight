#pragma once
#include "object.h"


// 충돌 처리 관계를 저장한 맵
std::unordered_map<std::string, std::pair<std::vector<Object*>, std::vector<Object*>>> collision_pairs;

void add_collision_pair(const std::string& group, Object* a, Object* b)
{
    // 그룹 이름에서 두 객체 타입 추출
    size_t colonPos = group.find(':');
    std::string type1 = group.substr(0, colonPos);
    std::string type2 = group.substr(colonPos + 1);

    if (collision_pairs.find(group) == collision_pairs.end())
    {
        collision_pairs[group] = std::make_pair(std::vector<Object*>(), std::vector<Object*>());
    }

    // a가 nullptr이 아니면 type1에, b가 nullptr이 아니면 type2에 추가
    if (a) collision_pairs[group].first.push_back(a);
    if (b) collision_pairs[group].second.push_back(b);
}
void remove_collision_object(Object* o, const std::string* group = nullptr)
{
    if (group)
    {
        auto it = collision_pairs.find(*group);
        if (it != collision_pairs.end())
        {
            auto& pairs = it->second;
            pairs.first.erase(std::remove(pairs.first.begin(), pairs.first.end(), o), pairs.first.end());
            pairs.second.erase(std::remove(pairs.second.begin(), pairs.second.end(), o), pairs.second.end());
        }
    }
    else
    {
        for (auto& pair : collision_pairs)
        {
            auto& group_pairs = pair.second;
            group_pairs.first.erase(std::remove(group_pairs.first.begin(), group_pairs.first.end(), o), group_pairs.first.end());
            group_pairs.second.erase(std::remove(group_pairs.second.begin(), group_pairs.second.end(), o), group_pairs.second.end());
        }
    }

    // 빈 그룹 제거
    auto it = collision_pairs.begin();
    while (it != collision_pairs.end())
    {
        if (it->second.first.empty() && it->second.second.empty())
        {
            it = collision_pairs.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
bool collide(const std::string& group, Object* a, Object* b)
{
    if (!a->is_active || !b->is_active)
    {
        remove_collision_object(a, &group);
        remove_collision_object(b, &group);
        std::cout << "DEBUG: " << a->is_active << ", " << b->is_active << " -> so deleted" << std::endl;
        return false;
    }

    // 3D 바운딩 박스를 가져옴
    BB bb_a = a->get_bb();
    BB bb_b = b->get_bb();

    // 3D 충돌 검사
    if (bb_a.top_left_front.x < bb_b.bottom_right_back.x) return false;
    if (bb_a.bottom_right_back.x > bb_b.top_left_front.x) return false;
    if (bb_a.top_left_front.y < bb_b.bottom_right_back.y) return false;
    if (bb_a.bottom_right_back.y > bb_b.top_left_front.y) return false;
    if (bb_a.top_left_front.z < bb_b.bottom_right_back.z) return false;
    if (bb_a.bottom_right_back.z > bb_b.top_left_front.z) return false;

    // 충돌 발생
    return true;
}
void handle_collisions()
{
    for (const auto& pair : collision_pairs)
    {
        const auto& group = pair.first;
        const auto& objects_a = pair.second.first;
        const auto& objects_b = pair.second.second;

        for (auto* a : objects_a)
        {
            if (!a->is_active) continue;

            for (auto* b : objects_b)
            {
                if (b->is_active && a != b)
                {
                    if (collide(group, a, b))
                    {
                        std::cout << group << " collide" << std::endl;
                        a->handle_collision(group, b);
                        b->handle_collision(group, a);
                    }
                }
            }
        }
    }
}