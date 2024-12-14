#pragma once
#pragma once
#include "model.h"
#include <glm/glm.hpp>
#include <vector>


class BackgroundTexture : public Model {
public:
    BackgroundTexture() : Model() {}
    float rotation_speed = 0.01f;
    unsigned int Texture;
    void init(const Model& model)
    {
        *static_cast<Model*>(this) = model;
        position_x = 0.0f;
        position_y = 10.0f;
        position_z = 20.0f;

        rotation_x = -26.57f;
        rotation_y = 0.0f;
        rotation_z = 0.0f;

        colors.clear();
        texcoord.clear();
        for (size_t i = 0; i < vertex_count; i++) {
            colors.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
            texcoord.push_back(glm::vec2((vertices[i].x * vertices[i].z + 1) / 2.0f, (vertices[i].y * vertices[i].z + 1) / 2.0f));
        }
        LoadTexture();
    }
    void update(float delta_time)
    {
        rotation_x += rotation_speed * delta_time*1000;//������Ʈ
    }

    void handle_event(int event_type, const char key, int special_key, int x, int y)
    {
        // �̺�Ʈ ó�� ����
    }
    void LoadTexture() {

        int width, height, channel; // BMP�� ����

        unsigned char* data = stbi_load("sky.bmp", &width, &height, &channel, 0); // BMP �ε�
        if (!data) {
            printf("Failed to load texture\n");
            return;
        }

        glGenTextures(1, &this->Texture);               // �ؽ�ó ����
        glBindTexture(GL_TEXTURE_2D, Texture);   // �ؽ�ó ���ε�

        // �ؽ�ó ������ ����
        GLenum format = (channel == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // �ؽ�ó �Ű����� ����
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        free(data); // �޸� ����
    }

};