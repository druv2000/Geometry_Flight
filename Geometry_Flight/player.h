//#pragma once
//#include "obj_reader.h"
//
//class Player : public Model
//{
//public:
//    // �� �� �̵��ӵ�
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
//        // ���� �ʱ�ȭ
//        colors.resize(vertex_count);
//        for (size_t i = 0; i < vertex_count; i++) {
//            colors[i] = glm::vec3(1.0f, 0.0f, 0.0f);  // �������� �ʱ�ȭ
//        }
//
//    }
//
//    void update()
//    {
//        // �� �����Ӹ��� �̵�
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
