#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include "bullet.h"
#include "bullet_pool.h"
#include "player.h"
#include "enemy.h"
#include "background.h"

#define CUBE 0
#define CYLINDER 1
#define CONE 2
#define SPHERE 3

#define ROTATE_NONE 0
#define ROTATE_X_PLUS 1
#define ROTATE_X_MINUS 2
#define ROTATE_Y_PLUS 3
#define ROTATE_Y_MINUS 4

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgramID;
GLuint vao, vbo[3];
GLuint ebo; // Element Buffer Object 추가

Model cubeModel, cylinderModel;
Model coneModel, sphereModel;

Player player;
std::vector<Enemy> enemies;
Background background;
std::vector<Object*> objects;
std::vector<Bullet> bullets;


glm::mat4 model, view, projection;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

bool drawModeSwitch = false;
int rotateSwitch = ROTATE_NONE;

float SCREEN_WIDTH = 800;
float SCREEN_HEIGHT = 900;

std::vector<int> selectedModels;

std::chrono::steady_clock::time_point last_update_time;
float frame_time = 0.0f;
float frame_rate = 0.0f;

BulletPool bulletPool(1000); // 100개의 총알을 가진 풀 생성


// --------- func ----------

void initializeModelColors(Model& model);
void draw_objects();

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
    glutCreateWindow("Geometry Flight");

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
    std::cerr << "cube.obj" << cubeModel.vertex_count << "  " << cubeModel.face_count << "  " << cubeModel.normal_count << std::endl;
    read_obj_file("cylinder.obj", &cylinderModel);
    std::cerr << "cylinderModel.obj" << cylinderModel.vertex_count << "  " << cylinderModel.face_count << "  " << cylinderModel.normal_count << std::endl;
    read_obj_file("cone.obj", &coneModel);
    std::cerr << "cone.obj" << coneModel.vertex_count << "  " << coneModel.face_count << "  " <<  coneModel.normal_count << std::endl;
    read_obj_file("sphere.obj", &sphereModel);
    std::cerr << "sphere.obj" << sphereModel.vertex_count << "  " << sphereModel.face_count << "  " << sphereModel.normal_count << std::endl;
    /*
    정점 법선 확인
    for (size_t i = 0; i < cubeModel.normals.size(); i++)std::cerr << "cube 법선" << i << ": (" << cubeModel.normals[i].x << ", " << cubeModel.normals[i].y << ", " << cubeModel.normals[i].z << ")" << std::endl;
    for (size_t i = 0; i < cylinderModel.normals.size(); i++)std::cerr << "cylinder법선" << i << ": (" << cylinderModel.normals[i].x << ", " << cylinderModel.normals[i].y << ", " << cylinderModel.normals[i].z << ")" << std::endl;
    for (size_t i = 0; i < coneModel.normals.size(); i++)std::cerr << "cone 법선" << i << ": (" << coneModel.normals[i].x << ", " << coneModel.normals[i].y << ", " << coneModel.normals[i].z << ")" << std::endl;
    for (size_t i = 0; i < sphereModel.normals.size(); i++)std::cerr << "sphere법선" << i << ": (" << sphereModel.normals[i].x << ", " << sphereModel.normals[i].y << ", " << sphereModel.normals[i].z << ")" << std::endl;
    std::cerr << "cube " << cubeModel.normals.size() << ", cylinder " << cylinderModel.normals.size() << ", cone " << coneModel.normals.size() << ", sphere " << sphereModel.normals.size() << std::endl;
    */
    initializeModelColors(cubeModel);
    initializeModelColors(cylinderModel);
    initializeModelColors(coneModel);
    initializeModelColors(sphereModel);

    objects.clear();  // 기존 모델 제거

    
    // 배경 설정
    background.init(cylinderModel, cubeModel);

    // 초기 모델 설정 (player)
    player.init(coneModel, 0.0f, 0.0f, 10.0f);
    objects.push_back(&player);


    initShapesBuffer();
    updateShapeBuffer();
    make_shaderProgram();

    selectedModels.clear();
    selectedModels.push_back(0);
    std::cout << "model[0] selected" << std::endl;

    last_update_time = std::chrono::steady_clock::now();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKeyboard);
    glutSpecialUpFunc(SpecialKeyboardUp);
    glutTimerFunc(0, Timer, 1);
    glutMainLoop();
}

void initializeModelColors(Model& model)
{
    model.colors.resize(model.vertex_count);
    for (size_t i = 0; i < model.vertex_count; i++) 
    {
        model.colors[i] = glm::vec3(dis(gen), dis(gen), dis(gen));
        //model.colors[i] = glm::vec3(1.0f, 1.0f, 1.0f);
    }
}
void draw_objects()
{
    // 배경의 인덱스 오프셋 계산
    unsigned int backgroundOffset = background.face_count * 3 + background.cube_models.size() * background.cube_models[0].face_count * 3;

    // 모델 그리기
    unsigned int indexOffset = backgroundOffset;
    for (size_t i = 0; i < objects.size(); i++)
    {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(objects[i]->position_x, objects[i]->position_y, objects[i]->position_z));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(objects[i]->rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(objects[i]->rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(objects[i]->rotation_z), glm::vec3(0.0f, 0.0f, 1.0f));

        if (i == 0) // 플레이어 캐릭터라면
        {
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
        }

        GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        glDrawElements(GL_TRIANGLES, objects[i]->face_count * 3, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(unsigned int)));
        indexOffset += objects[i]->face_count * 3;
    }
}
void draw_bullets() 
{
    // 배경의 인덱스 오프셋 계산
    unsigned int backgroundOffset = background.face_count * 3 + background.cube_models.size() * background.cube_models[0].face_count * 3;

    // 다른 오브젝트들의 인덱스 오프셋 계산
    unsigned int objectsOffset = backgroundOffset;
    for (const auto& model : objects) 
    {
        objectsOffset += model->face_count * 3;
    }

    for (const auto& bullet : bulletPool.getAllBullets()) 
    {
        if (bullet.is_active)
        {
            glm::mat4 bulletModel = glm::mat4(1.0f);
            bulletModel = glm::translate(bulletModel, glm::vec3(bullet.position_x, bullet.position_y, bullet.position_z));
            bulletModel = glm::scale(bulletModel, glm::vec3(0.2f, 0.2f, 0.2f)); // 크기 조절

            GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bulletModel));

            // 총알의 인덱스 오프셋 계산
            GLuint indexOffset = objectsOffset;
            for (const auto& b : bulletPool.getAllBullets()) 
            {
                if (&b == &bullet) break;
                if (b.is_active) indexOffset += b.face_count * 3;
            }

            glDrawElements(GL_TRIANGLES, bullet.face_count * 3, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(GLuint)));
        }
    }
}
void draw_background()
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(background.position_x, background.position_y, background.position_z));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(background.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(background.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(background.rotation_z), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f, 30.0f, 20.0f));

    // 배경 실린더 그리기
    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glDrawElements(GL_TRIANGLES, background.face_count * 3, GL_UNSIGNED_INT, 0);

    // 작은 정육면체들 그리기
    for (const auto& cube : background.cube_models)
    {
        glm::mat4 cubeMatrix = modelMatrix;
        cubeMatrix = glm::translate(cubeMatrix, glm::vec3(cube.position_x, cube.position_y, cube.position_z));
        cubeMatrix = glm::rotate(cubeMatrix, glm::radians(cube.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
        cubeMatrix = glm::rotate(cubeMatrix, glm::radians(cube.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
        cubeMatrix = glm::rotate(cubeMatrix, glm::radians(cube.rotation_z), glm::vec3(0.0f, 0.0f, 1.0f));
        cubeMatrix = glm::scale(cubeMatrix, glm::vec3(0.1f, 0.1f, 0.5f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeMatrix));
        glDrawElements(GL_TRIANGLES, cube.face_count * 3, GL_UNSIGNED_INT, (void*)(background.face_count * 3 * sizeof(unsigned int)));
    }
}
void updateShapeBuffer() 
{
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normalinf;
    std::vector<unsigned int> normals;
    std::vector<glm::vec3> normalresult;
    unsigned int vertexOffset = 0;
    unsigned int normalOffset = 0;
    // 배경 실린더 버퍼 업데이트
    const Model& backgroundCylinder = static_cast<Model>(background);

    for (size_t i = 0; i < backgroundCylinder.normal_count; i++) {
        normalinf.push_back(backgroundCylinder.normals[i]);
    }
    for (size_t i = 0; i < backgroundCylinder.vertex_count; i++) 
    {
        vertices.push_back(glm::vec3(backgroundCylinder.vertices[i].x, backgroundCylinder.vertices[i].y, backgroundCylinder.vertices[i].z));
        colors.push_back(backgroundCylinder.colors[i]);
    }
    for (size_t i = 0; i < backgroundCylinder.face_count; i++) 
    {
        indices.push_back(backgroundCylinder.faces[i].v1 - 1 + vertexOffset);
        indices.push_back(backgroundCylinder.faces[i].v2 - 1 + vertexOffset);
        indices.push_back(backgroundCylinder.faces[i].v3 - 1 + vertexOffset);
        normals.push_back(backgroundCylinder.faces[i].n1 - 1 + normalOffset);
        normals.push_back(backgroundCylinder.faces[i].n2 - 1 + normalOffset);
        normals.push_back(backgroundCylinder.faces[i].n3 - 1 + normalOffset);
    }
    
    normalOffset += backgroundCylinder.normal_count;
    vertexOffset += backgroundCylinder.vertex_count;

    // 작은 정육면체들 버퍼 업데이트
    for (const auto& cube : background.cube_models) 
    {
        for (size_t i = 0; i < cube.normal_count; i++) {
            normalinf.push_back(cube.normals[i]);
        }
        for (size_t i = 0; i < cube.vertex_count; i++) 
        {
            vertices.push_back(glm::vec3(cube.vertices[i].x, cube.vertices[i].y, cube.vertices[i].z));
            colors.push_back(cube.colors[i]);
        }
        for (size_t i = 0; i < cube.face_count; i++) 
        {
            indices.push_back(cube.faces[i].v1 - 1 + vertexOffset);
            indices.push_back(cube.faces[i].v2 - 1 + vertexOffset);
            indices.push_back(cube.faces[i].v3 - 1 + vertexOffset);
            normals.push_back(cube.faces[i].n1 - 1 + normalOffset);
            normals.push_back(cube.faces[i].n2 - 1 + normalOffset);
            normals.push_back(cube.faces[i].n3 - 1 + normalOffset);

        }
        normalOffset += cube.normal_count;
        vertexOffset += cube.vertex_count;
    }

   // objects 벡터의 모든 객체 업데이트 (플레이어, 적 등)
    for (const auto& object : objects)
    {
        for (size_t i = 0; i < object->normal_count; i++) {
            normalinf.push_back(object->normals[i]);
        }
        for (size_t i = 0; i < object->vertex_count; i++)
        {
            const Vertex& vertex = object->vertices[i];
            vertices.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));
            colors.push_back(object->colors[i]);
        }
        for (size_t i = 0; i < object->face_count; i++)
        {
            const Face& face = object->faces[i];
            indices.push_back(face.v1 - 1 + vertexOffset);
            indices.push_back(face.v2 - 1 + vertexOffset);
            indices.push_back(face.v3 - 1 + vertexOffset);
            normals.push_back(face.n1 - 1 + normalOffset);
            normals.push_back(face.n2 - 1 + normalOffset);
            normals.push_back(face.n3 - 1 + normalOffset);
        }
        normalOffset += object->normal_count;
        vertexOffset += object->vertex_count;
    }

    // 총알 버퍼 업데이트
    for (const auto& bullet : bulletPool.getAllBullets()) 
    {
        if (bullet.is_active) {
            for (size_t i = 0; i < bullet.normal_count; i++) {
                normalinf.push_back(bullet.normals[i]);
            }
            for (size_t i = 0; i < bullet.vertex_count; i++) 
            {
                vertices.push_back(glm::vec3(bullet.vertices[i].x, bullet.vertices[i].y, bullet.vertices[i].z));
                colors.push_back(bullet.colors[i]);
            }
            for (size_t i = 0; i < bullet.face_count; i++)
            {
                indices.push_back(bullet.faces[i].v1 - 1 + vertexOffset);
                indices.push_back(bullet.faces[i].v2 - 1 + vertexOffset);
                indices.push_back(bullet.faces[i].v3 - 1 + vertexOffset);
                normals.push_back(bullet.faces[i].n1 - 1 + normalOffset );
                normals.push_back(bullet.faces[i].n2 - 1 + normalOffset);
                normals.push_back(bullet.faces[i].n3 - 1 + normalOffset);
            }
            normalOffset += bullet.normal_count;
            vertexOffset += bullet.vertex_count;
        }
    }
    //문제의 구간
    //법선의 정보와 인데스를 맞춘다
    for (size_t i = 0; i < normals.size(); i++)
    {
        normalresult.push_back(normalinf[normals[i]]);
    }
    




    //색의 변화를 알기 쉽게 모두 단색으로
    size_t temp = vertexOffset;
    colors.clear();
    for (size_t i = 0; i < temp; i++)
    {
        colors.push_back(glm::vec3(1.0f,1.0f,1.0f));
    }
    
    
   

    // 버퍼 데이터 업데이트
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, normalresult.size() * sizeof(glm::vec3), normalresult.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}
void initShapesBuffer()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(3, vbo);
    glGenBuffers(1, &ebo);

    updateShapeBuffer();

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(2);

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


    glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), 0.0f, 10.0f, 10.0f);//광원의 위치
    glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), 0.8f, 0.8f, 0.8f);//빛의 색
    glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), 0.0f, 10.0f, 20.0f);//카메라 위치
    draw_background();
    draw_objects();
    draw_bullets();
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
    {
        Enemy* new_enemy = new Enemy();
        new_enemy->init(cylinderModel, 0.0f, 0.0f, -50.0f);
        objects.push_back(new_enemy);

        updateShapeBuffer();
        break;
    }
    case ' ':
    {
        // Bullet 객체 생성 및 추가
        Model bulletModel = sphereModel;
        Bullet* newBullet = bulletPool.getBullet(bulletModel, -0.1f);
        if (newBullet != nullptr)
        {
            // 총알 초기 위치 설정 등 추가 로직
            newBullet->position_x = player.position_x;
            newBullet->position_y = player.position_y;
            newBullet->position_z = player.position_z;
        }

        //Model bulletModel2 = sphereModel;
        //Bullet* newBullet2 = bulletPool.getBullet(bulletModel2, -0.1f);
        //if (newBullet != nullptr)
        //{
        //    // 총알 초기 위치 설정 등 추가 로직
        //    newBullet2->position_x = player.position_x;
        //    newBullet2->position_y = player.position_y;
        //    newBullet2->position_z = player.position_z;
        //}

        //Model bulletModel3 = sphereModel;
        //Bullet* newBullet3 = bulletPool.getBullet(bulletModel3, -0.1f);
        //if (newBullet != nullptr)
        //{
        //    // 총알 초기 위치 설정 등 추가 로직
        //    newBullet3->position_x = player.position_x;
        //    newBullet3->position_y = player.position_y;
        //    newBullet3->position_z = player.position_z;
        //}

        updateShapeBuffer();
    }
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
    player.handle_event(SPECIAL_KEYBOARD_KEYDOWN, '0', key, x, y);
    glutPostRedisplay();
}
GLvoid SpecialKeyboardUp(int key, int x, int y)
{
    player.handle_event(SPECIAL_KEYBOARD_KEYUP, '0', key, x, y);
    glutPostRedisplay();
}
GLvoid Timer(int value)
{
    Update();
    glutTimerFunc(1, Timer, 1);
}
GLvoid Update()
{
    auto current_time = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = current_time - last_update_time;
    frame_time = elapsed.count();
    frame_rate = 1.0f / frame_time;
    
    // 배경 업데이트
    background.update(frame_time);

    // 오브젝트(플레이어, 적) 업데이트
    int i = 0;
    for (auto& object : objects)
    {
        object->update(frame_time);
        objects[i] = object;
        i++;
        std::cout << objects.size() << std::endl;
    }

    // 총알 업데이트
    bulletPool.update(frame_time);


    // 프레임 시간과 프레임 레이트 출력
    std::cout << "Frame Time: " << frame_time * 1000.0f << " ms, Frame Rate: " << frame_rate << " FPS\n";
    last_update_time = current_time;

    objects.erase(std::remove_if(objects.begin(), objects.end(),
        [](Object* obj)
        {
            Enemy* enemy = dynamic_cast<Enemy*>(obj);
            if (enemy && !enemy->is_active)
            {
                delete enemy;
                return true;
            }
            return false;
        }), objects.end());

    updateShapeBuffer();
    glutPostRedisplay();
}