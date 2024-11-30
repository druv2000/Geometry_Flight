//#pragma once
//#include "obj_reader.h"
//
//class Player : public Model
//{
//public:
//    // 각 축 이동속도
//    float xSpeed;
//    float ySpeed;
//    float zSpeed;
//
//    float initial_pos_x;
//    float initial_pos_y;
//    float initial_pos_z;
//
//    Player(const Model model, float pos_x, float pos_y, float pos_z): 
//        Model(model), initial_pos_x(pos_x), initial_pos_y(pos_y), initial_pos_z(pos_z)
//    {
//        // 색상 초기화
//        colors.resize(vertex_count);
//        for (size_t i = 0; i < vertex_count; i++) {
//            colors[i] = glm::vec3(1.0f, 0.0f, 0.0f);  // 빨강으로 초기화
//        }
//
//    }
//
//    void update()
//    {
//        // 매 프레임마다 이동
//        positionX += xSpeed;
//        positionX += ySpeed;
//        positionZ += zSpeed;
//    }
//
//    void handle_event()
//    {
//
//    }
//};
//
