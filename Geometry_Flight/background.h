#pragma once
#include "model.h"
#include <glm/glm.hpp>
#include <vector>

#define NORMAL_KEYBOARD_KEYDOWN 0
#define NORMAL_KEYBOARD_KEYUP 1
#define SPECIAL_KEYBOARD_KEYDOWN 2
#define SPECIAL_KEYBOARD_KEYUP 3

class Background : public Model {
public:
    Background() : Model() {}
    float rotation_speed = 0.1f;
    std::vector<Model> cube_models;
    unsigned int cubetexture, cylindertexture;
    void init(const Model& model, const Model& cube)
    {
        *static_cast<Model*>(this) = model;
        position_x = 0.0f;
        position_y = -40.0f;
        position_z = -20.0f;

        rotation_x = 0.0f;
        rotation_y = 0.0f;
        rotation_z = 90.0f;

        for (size_t i = 0; i < vertex_count; i++)
        {
            texcoord.push_back(glm::vec2((vertices[i].x * vertices[i].z + 1) / 2.0f, (vertices[i].y * vertices[i].z + 1) / 2.0f));
        }
        LoadTexture();
        generate_random_cubes(100, cube); // 50���� ť�� ����
    }

    void update(float delta_time)
    {
        this->rotation_x += rotation_speed * delta_time * 1000; // 1000fps ����
    }

    void handle_event(int event_type, const char key, int special_key, int x, int y)
    {
        // �̺�Ʈ ó�� ����
    }

    void generate_random_cubes(int count, const Model& cube_template)
    {
        cube_models.clear();
        for (int i = 0; i < count; ++i)
        {
            // ������ �������� �������Ƿ� height, radius ���� ���� ���¸� �����ϰ� �����ؾ� ��
            float theta = glm::radians(glm::linearRand(0.0f, 360.0f));
            float height;                                   // �ǹ� ��ġ�� ������ �ʺ� (-1 ~ 1: ������ �� �� ����)
            float radius = glm::linearRand(0.75f, 1.4f);    // �ǹ� ��ġ�� ������ ���� (1.5: ǥ�鿡 ����)

            // �߾��� ���� ����
            if (glm::linearRand(0.0f, 1.0f) < 0.5f)
            {
                height = glm::linearRand(0.4f, 1.0f);
            }
            else
            {
                height = glm::linearRand(-1.0f, -0.4f);
            }

            glm::vec3 position(
                radius * cos(theta),
                height,
                radius * sin(theta)
            );

            Model cube = cube_template;
            cube.position_x = position.x;
            cube.position_y = position.y;
            cube.position_z = position.z;

            // �Ǹ��� �߽��� ���ϴ� ȸ�� ���
            float rotation_y = glm::degrees(atan2(position.x, position.z));
            cube.rotation_y = rotation_y;
            cube.rotation_x = 0.0f;
            cube.rotation_z = 0.0f;

            //// ���� ������ �߰�
            //float scale = glm::linearRand(0.05f, 0.15f);
            //cube.scale_x = scale;
            //cube.scale_y = scale;
            //cube.scale_z = scale;

            //�ؽ��� ��ǥ �߰�
            for (size_t i = 0; i < cube.vertex_count; i++)
            {
                cube.texcoord.push_back(glm::vec2((cube.vertices[i].x * cube.vertices[i].z + 1) / 2.0f, (cube.vertices[i].y * cube.vertices[i].z + 1) / 2.0f));
            }
            cube_models.push_back(cube);
        }
    }


    void LoadTexture() {

        int width, height, channel; // BMP�� ����

        unsigned char* data = stbi_load("brick.bmp", &width, &height, &channel, 0); // BMP �ε�
        if (!data) {
            printf("Failed to load brick texture\n");
            return;
        }

        glGenTextures(1, &this->cubetexture);               // �ؽ�ó ����
        glBindTexture(GL_TEXTURE_2D, cubetexture);   // �ؽ�ó ���ε�

        // �ؽ�ó ������ ����
        GLenum format = (channel == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // �ؽ�ó �Ű����� ����
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        data = stbi_load("asphalt.bmp", &width, &height, &channel, 0); // BMP �ε�
        if (!data) {
            printf("Failed to load asphalt texture\n");
            return;
        }

        glGenTextures(1, &this->cylindertexture);               // �ؽ�ó ����
        glBindTexture(GL_TEXTURE_2D, cylindertexture);   // �ؽ�ó ���ε�

        // �ؽ�ó ������ ����
        format = (channel == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // �ؽ�ó �Ű����� ����
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        free(data); // �޸� ����
    }
};