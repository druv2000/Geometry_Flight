#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utility.h"
#include <random>
#include <chrono>
#include <thread>
#include <cmath>

#include "bullet.h"

#define CUBE 0
#define CYLINDER 1
#define CONE 2
#define SPHERE 3

#define ROTATE_NONE 0
#define ROTATE_X_PLUS 1
#define ROTATE_X_MINUS 2
#define ROTATE_Y_PLUS 3
#define ROTATE_Y_MINUS 4

#define ORBIT_NONE 0
#define ORBIT_PLUS 1
#define ORBIT_MINUS 2

#define DISTANCE_BY_CENTER 3.0f;


GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgramID;
GLuint vao, vbo[2];
GLuint ebo; // Element Buffer Object 추가

Model cubeModel, cylinderModel;
Model coneModel, sphereModel;

std::vector<Model> models;
std::vector<Bullet> bullets;


glm::mat4 model, view, projection;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

bool drawModeSwitch = false;
int rotateSwitch = ROTATE_NONE;
float moveX = 0.0f;
float moveY = 0.0f;

float SCREEN_WIDTH = 800;
float SCREEN_HEIGHT = 900;

std::vector<int> selectedModels;

// --------- func ---------

void initializeModelColors(Model& model);
void resetModels();
void drawModels();

void updateShapeBuffer();
void initShapesBuffer();
void make_vertexShaders();
void make_fragmentShader();
GLint make_shaderProgram();

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeyboard(int key, int x, int y);
GLvoid SpecialKeyboardUp(int key, int x, int y);
GLvoid Timer(int value);
GLvoid Update();

// ==================== main =============================

void main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("TASK_15");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "GLEW initialized\n";
    }

    // cube, cylinder, cone, sphere 모델 불러오기
    read_obj_file("cube.obj", &cubeModel);
    read_obj_file("cylinder.obj", &cylinderModel);
    read_obj_file("cone.obj", &coneModel);
    read_obj_file("sphere.obj", &sphereModel);
    initializeModelColors(cubeModel);
    initializeModelColors(cylinderModel);
    initializeModelColors(coneModel);
    initializeModelColors(sphereModel);

    // 초기 모델을 cube와 cone로 설정
    models.push_back(cubeModel);
    models.push_back(coneModel);

    resetModels();
    initShapesBuffer();
    make_shaderProgram();

    selectedModels.clear();
    selectedModels.push_back(0);
    std::cout << "model[0] selected" << std::endl;

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKeyboard);
    glutSpecialUpFunc(SpecialKeyboardUp);
    glutTimerFunc(0, Timer, 1);
    glutMainLoop();
}

void initializeModelColors(Model& model) {
    model.colors.resize(model.vertex_count);
    for (size_t i = 0; i < model.vertex_count; i++) {
        model.colors[i] = glm::vec3(dis(gen), dis(gen), dis(gen));
        //model.colors[i] = glm::vec3(1.0f, 1.0f, 1.0f);
    }
}
void resetModels()
{
    // 회전 종료
    rotateSwitch = ROTATE_NONE;

    // models[0] 위치, 회전정도 초기화
    models[0].positionX = 0.0f;
    models[0].positionY = 0.0f;
    models[0].positionZ = -20.0f;
    models[0].rotationX = 90.0f;
    models[0].rotationY = 0.0f;

    // models[1] 위치, 회전정도 초기화
    models[1].positionX = 0.0;
    models[1].positionY = 0.0f;
    models[1].positionZ = 10.0f;
    models[1].rotationX = -90.0f;
    models[1].rotationY = 0.0f;
}
void drawModels() {
    // 모델 그리기
    unsigned int indexOffset = 0;
    for (size_t i = 0; i < models.size(); i++) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(models[i].positionX, models[i].positionY, models[i].positionZ));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(models[i].rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(models[i].rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

        GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        glDrawElements(GL_TRIANGLES, models[i].face_count * 3, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(unsigned int)));
        indexOffset += models[i].face_count * 3;
    }
}
void drawBullets()
{
    // 총알 그리기
    for (const auto& bullet : bullets) {
        glm::mat4 bulletModel = glm::mat4(1.0f);
        bulletModel = glm::translate(bulletModel, glm::vec3(bullet.positionX, bullet.positionY, bullet.positionZ));
        bulletModel = glm::scale(bulletModel, glm::vec3(0.2f, 0.2f, 0.2f));  // 크기 조절

        GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bulletModel));

        // sphereModel의 인덱스 오프셋 계산
        GLuint indexOffset = 0;
        for (const auto& model : models) {
            if (&model == &sphereModel) break;
            indexOffset += model.face_count * 3;
        }

        glDrawElements(GL_TRIANGLES, sphereModel.face_count * 3, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(GLuint)));
    }
}

void updateShapeBuffer() {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> colors;

    // 모델 버퍼 업데이트
    unsigned int vertexOffset = 0;
    for (const auto& model : models) {
        for (size_t i = 0; i < model.vertex_count; i++) {
            vertices.push_back(glm::vec3(model.vertices[i].x, model.vertices[i].y, model.vertices[i].z));
            colors.push_back(model.colors[i]);  // 저장된 색상 사용
        }
        for (size_t i = 0; i < model.face_count; i++) {
            indices.push_back(model.faces[i].v1 - 1 + vertexOffset);
            indices.push_back(model.faces[i].v2 - 1 + vertexOffset);
            indices.push_back(model.faces[i].v3 - 1 + vertexOffset);
        }
        vertexOffset += model.vertex_count;
    }

    // 총알 버퍼 업데이트
    for (const auto& bullet : bullets) {
        for (size_t i = 0; i < bullet.vertex_count; i++) {
            vertices.push_back(glm::vec3(bullet.vertices[i].x, bullet.vertices[i].y, bullet.vertices[i].z));
            colors.push_back(bullet.colors[i]);  // 총알의 저장된 색상 사용
        }
        for (size_t i = 0; i < bullet.face_count; i++) {
            indices.push_back(bullet.faces[i].v1 - 1 + vertexOffset);
            indices.push_back(bullet.faces[i].v2 - 1 + vertexOffset);
            indices.push_back(bullet.faces[i].v3 - 1 + vertexOffset);
        }
        vertexOffset += bullet.vertex_count;
    }

    // 버퍼 데이터 업데이트
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
}
void initShapesBuffer()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(2, vbo);
    glGenBuffers(1, &ebo);

    updateShapeBuffer();

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}
void make_vertexShaders()
{
    GLchar* vertexSource = fileToBuf("vertex.glsl");

    // vertex 셰이더 객체 생성 -> 셰이더 코드 삽입 -> 컴파일
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);


    // 컴파일 과정 에러 체크
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패" << std::endl << errorLog << std::endl;
        return;
    }
}
void make_fragmentShader()
{
    GLchar* fragmentSource = fileToBuf("fragment.glsl");

    // fragment 셰이더 객체 생성 -> 셰이더 코드 삽입 -> 컴파일
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // 컴파일 과정 에러 체크
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader 컴파일 실패" << std::endl << errorLog << std::endl;
        return;
    }
}
GLint make_shaderProgram()
{
    // vertex, fragment 셰이더 만들기
    make_vertexShaders();
    make_fragmentShader();

    // 셰이더 프로그램
    shaderProgramID = glCreateProgram();

    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 연결 과정에서 에러 확인
    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderProgramID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패" << std::endl << errorLog << std::endl;
        return false;
    }

    // 셰이더 프로그램 사용
    glUseProgram(shaderProgramID);
}

GLvoid drawScene()
{
    if (drawModeSwitch) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);
    glBindVertexArray(vao);

    float fov = 45.0f;

    view = glm::lookAt(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(fov), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgramID, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    drawModels();
    drawBullets();
    glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case '1':
        selectedModels.clear();
        selectedModels.push_back(0);
        std::cout << "model[0] selected" << std::endl;
        break;
    case '2':
        selectedModels.clear();
        selectedModels.push_back(1);
        std::cout << "model[1] selected" << std::endl;
        break;
    case '3':
        selectedModels.clear();
        selectedModels.push_back(0);
        selectedModels.push_back(1);
        std::cout << "model[0], model[1] selected" << std::endl;
        break;
    case 'x':
        rotateSwitch = ROTATE_X_PLUS;
        break;
    case 'X':
        rotateSwitch = ROTATE_X_MINUS;
        break;
    case 'y':
        rotateSwitch = ROTATE_Y_PLUS;
        break;
    case 'Y':
        rotateSwitch = ROTATE_Y_MINUS;
        break;
    case ' ':
    {
        std::cout << "새로운 총알이 발사되었습니다!" << std::endl;
        Model newBullet = sphereModel;
        newBullet.positionX = models[1].positionX;
        newBullet.positionY = models[1].positionY;
        newBullet.positionZ = models[1].positionZ;

        // Bullet 객체 생성 및 추가
        Bullet Bullet(newBullet, -0.1f);  // 초기 속도 0.1f로 설정
        bullets.push_back(Bullet);

        updateShapeBuffer();
    }
    break;
    break;
    case 'q':
        exit(0);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}
GLvoid SpecialKeyboard(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_LEFT:
        moveX = -0.01f;
        break;
    case GLUT_KEY_RIGHT:
        moveX = 0.01f;
        break;
    }
    glutPostRedisplay();
}
GLvoid SpecialKeyboardUp(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_LEFT:
    case GLUT_KEY_RIGHT:
        moveX = 0.0f;
        break;
    }
    glutPostRedisplay();
}
GLvoid Timer(int value)
{
    Update();
    glutTimerFunc(1, Timer, 1);
}
GLvoid Update()
{
    for (int modelIdx : selectedModels) // 선택된 도형들에 한해서 실행
    {
        // 회전 처리
        Model* currentModel = &models[modelIdx];
        switch (rotateSwitch) {
        case ROTATE_X_PLUS:
            currentModel->rotationX += 5.0f;
            break;
        case ROTATE_X_MINUS:
            currentModel->rotationX -= 5.0f;
            break;
        case ROTATE_Y_PLUS:
            currentModel->rotationY += 5.0f;
            break;
        case ROTATE_Y_MINUS:
            currentModel->rotationY -= 5.0f;
            break;
        }

        // 이동 처리
        currentModel->positionX += moveX;
        currentModel->positionY += moveY;

        // 각도가 360도를 넘어가면 0으로 리셋
        currentModel->rotationX = fmod(currentModel->rotationX, 360.0f);
        currentModel->rotationY = fmod(currentModel->rotationY, 360.0f);
    }

    // 총알 업데이트
    for (auto& bullet : bullets) {
        bullet.update();
    }

    // 화면 밖으로 나간 총알 제거
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return b.positionZ > 100.0f; }),
        bullets.end()
    );

    updateShapeBuffer();
    glutPostRedisplay();
}