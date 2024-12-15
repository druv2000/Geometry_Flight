#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "bullet.h"
#include "bullet_pool.h"
#include "player.h"
#include "enemy.h"
#include "background.h"
#include "backgroundtexture.h"
#include "obj_reader.h"
#include "collision_manager.h"
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
GLuint vao, vbo[4];
GLuint ebo; // Element Buffer Object �߰�

// ���� ��
Model cubeModel, cylinderModel;
Model coneModel, sphereModel;

// ���� ������Ʈ��
Player player;
std::vector<Enemy> enemies;
Background background;
BackgroundTexture backgroundtexture;
std::vector<Object*> objects;

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

BulletPool bulletPool(1000); // 100���� �Ѿ��� ���� Ǯ ����

bool draw_bb_switch = false;
bool slow_switch = false;


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
GLvoid enemy_wave_manager(int value);


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

    // cube, cylinder, cone, sphere �� �ҷ�����
    read_obj_file("cube.obj", &cubeModel);
    //std::cerr << "cube: " << cubeModel.vertex_count << "  " << cubeModel.face_count << "  " << cubeModel.normal_count << std::endl;
    read_obj_file("cylinder.obj", &cylinderModel);
    //std::cerr << "cylinder" << cylinderModel.vertex_count << "  " << cylinderModel.face_count << "  " << cylinderModel.normal_count << std::endl;
    read_obj_file("cone.obj", &coneModel);
    //std::cerr << "cone: " << coneModel.vertex_count << "  " << coneModel.face_count << "  " <<  coneModel.normal_count << std::endl;
    read_obj_file("sphere.obj", &sphereModel);
    //std::cerr << "sphere: " << sphereModel.vertex_count << "  " << sphereModel.face_count << "  " << sphereModel.normal_count << std::endl;
    
    glEnable(GL_DEPTH_TEST);
    initializeModelColors(cubeModel);
    initializeModelColors(cylinderModel);
    initializeModelColors(coneModel);
    initializeModelColors(sphereModel);

    objects.clear();  // ���� �� ����

    
    // ��� ����
    background.init(cylinderModel, cubeModel);
    backgroundtexture.init(cubeModel);
    // �ʱ� �� ���� (player)
    player.init(coneModel, 0.0f, 0.0f, 10.0f);
    player.bb = player.get_bb();
    objects.push_back(&player);
    add_collision_pair("player:enemy", &player, nullptr);


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
    glutTimerFunc(1000, enemy_wave_manager, 0);
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
    // ����� �ε��� ������ ���
    unsigned int backgroundOffset = background.face_count * 3 + background.cube_models.size() * background.cube_models[0].face_count * 3 + backgroundtexture.face_count * 3;

    // �� �׸���
    unsigned int indexOffset = backgroundOffset;
    for (const auto& object: objects)
    {
       if (object->is_active)
       {
           glm::mat4 modelMatrix = glm::mat4(1.0f);
           modelMatrix = glm::translate(modelMatrix, glm::vec3(object->position_x, object->position_y, object->position_z));
           modelMatrix = glm::rotate(modelMatrix, glm::radians(object->rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
           modelMatrix = glm::rotate(modelMatrix, glm::radians(object->rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
           modelMatrix = glm::rotate(modelMatrix, glm::radians(object->rotation_z), glm::vec3(0.0f, 0.0f, 1.0f));

           if (object->type == TYPE_PLAYER) // �÷��̾� ĳ���Ͷ��
           {
               modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
               // �߰��� �ʿ��� ó���� �� �� ����
               // ȸ�� ó���� ���⿡ ������ �����Ű�����, ���� player.init()���� ó������ �ʾƵ� �ǰ�
               // �׷��� �̰� �� drawScene���� ����Ǵ°ǵ�, ���⼭ ó���ϸ� ������ �þ�°ǰ�?
           }
           else if (object->type == TYPE_BULLET_1) // �Ѿ�_1 �̶��
           {
               modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
           }
           else if (object->type == TYPE_ENEMY_1) // ��_1 �̶��
           {
               modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
           }
           else if (object->type == TYPE_ENEMY_1) {
               modelMatrix = glm::scale(modelMatrix, glm::vec3(50.0f, 50.0f, 50.0f));
           }

           GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
           glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

           glDrawElements(GL_TRIANGLES, object->face_count * 3, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(unsigned int)));
           indexOffset += object->face_count * 3;
       }
    }
}
void draw_background()
{
    unsigned int indexOffset = backgroundtexture.face_count * 3;

    glBindTexture(GL_TEXTURE_2D, background.cylindertexture);

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(background.position_x, background.position_y, background.position_z));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(background.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(background.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(background.rotation_z), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f, 30.0f, 20.0f));

    // ��� �Ǹ��� �׸���
    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glDrawElements(GL_TRIANGLES, background.face_count * 3, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(unsigned int)));
    indexOffset += background.face_count * 3;

    // ���� ������ü�� �׸���
    glBindTexture(GL_TEXTURE_2D, background.cubetexture);
    for (const auto& cube : background.cube_models)
    {
        glm::mat4 cubeMatrix = modelMatrix;
        cubeMatrix = glm::translate(cubeMatrix, glm::vec3(cube.position_x, cube.position_y, cube.position_z));
        cubeMatrix = glm::rotate(cubeMatrix, glm::radians(cube.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
        cubeMatrix = glm::rotate(cubeMatrix, glm::radians(cube.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
        cubeMatrix = glm::rotate(cubeMatrix, glm::radians(cube.rotation_z), glm::vec3(0.0f, 0.0f, 1.0f));
        cubeMatrix = glm::scale(cubeMatrix, glm::vec3(0.1f, 0.1f, 0.5f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeMatrix));

        glDrawElements(GL_TRIANGLES, cube.face_count * 3, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(unsigned int)));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}
void draw_backgroundtexture()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    unsigned int indexOffset = background.face_count * 3 + background.cube_models.size() * background.cube_models[0].face_count * 3;

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(backgroundtexture.position_x, backgroundtexture.position_y, backgroundtexture.position_z));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(backgroundtexture.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(backgroundtexture.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(backgroundtexture.rotation_z), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(30.0f, 70.0f, 70.0f));

    // ��� ť�� �׸���
    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glBindTexture(GL_TEXTURE_2D, backgroundtexture.Texture);
    glDrawElements(GL_TRIANGLES, backgroundtexture.face_count * 3, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_CULL_FACE);

}
void draw_object_bb() {
    glUseProgram(shaderProgramID);

    // ���� ���� ���� (��: ������)
    glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 0.0f, 0.0f);

    for (const auto& object : objects) {
        if (object->is_active) {
            BB bb = object->get_bb();

            // �ٿ�� �ڽ��� 8�� ������ ���
            glm::vec3 vertices[8] = {
                glm::vec3(bb.top_left_front.x, bb.top_left_front.y, bb.top_left_front.z),
                glm::vec3(bb.bottom_right_back.x, bb.top_left_front.y, bb.top_left_front.z),
                glm::vec3(bb.bottom_right_back.x, bb.bottom_right_back.y, bb.top_left_front.z),
                glm::vec3(bb.top_left_front.x, bb.bottom_right_back.y, bb.top_left_front.z),
                glm::vec3(bb.top_left_front.x, bb.top_left_front.y, bb.bottom_right_back.z),
                glm::vec3(bb.bottom_right_back.x, bb.top_left_front.y, bb.bottom_right_back.z),
                glm::vec3(bb.bottom_right_back.x, bb.bottom_right_back.y, bb.bottom_right_back.z),
                glm::vec3(bb.top_left_front.x, bb.bottom_right_back.y, bb.bottom_right_back.z)
            };

            // �� ��� ����
            glm::mat4 model = glm::mat4(1.0f);
            GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // �ٿ�� �ڽ� �׸���
            glBegin(GL_LINES);
            // �ո�
            glVertex3fv(glm::value_ptr(vertices[0])); glVertex3fv(glm::value_ptr(vertices[1]));
            glVertex3fv(glm::value_ptr(vertices[1])); glVertex3fv(glm::value_ptr(vertices[2]));
            glVertex3fv(glm::value_ptr(vertices[2])); glVertex3fv(glm::value_ptr(vertices[3]));
            glVertex3fv(glm::value_ptr(vertices[3])); glVertex3fv(glm::value_ptr(vertices[0]));
            // �޸�
            glVertex3fv(glm::value_ptr(vertices[4])); glVertex3fv(glm::value_ptr(vertices[5]));
            glVertex3fv(glm::value_ptr(vertices[5])); glVertex3fv(glm::value_ptr(vertices[6]));
            glVertex3fv(glm::value_ptr(vertices[6])); glVertex3fv(glm::value_ptr(vertices[7]));
            glVertex3fv(glm::value_ptr(vertices[7])); glVertex3fv(glm::value_ptr(vertices[4]));
            // ���ἱ
            glVertex3fv(glm::value_ptr(vertices[0])); glVertex3fv(glm::value_ptr(vertices[4]));
            glVertex3fv(glm::value_ptr(vertices[1])); glVertex3fv(glm::value_ptr(vertices[5]));
            glVertex3fv(glm::value_ptr(vertices[2])); glVertex3fv(glm::value_ptr(vertices[6]));
            glVertex3fv(glm::value_ptr(vertices[3])); glVertex3fv(glm::value_ptr(vertices[7]));
            glEnd();
        }
    }
}

void updateShapeBuffer() 
{
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec2> Textures;

    std::vector<glm::vec3> normalinf;



    unsigned int vertexOffset = 0, normalOffset = 0;

    //��� ť��
    const Model& backgroundCube = static_cast<Model>(backgroundtexture);

    for (size_t i = 0; i < backgroundCube.vertex_count; i++)
    {
        vertices.push_back(glm::vec3(backgroundCube.vertices[i].x, backgroundCube.vertices[i].y, backgroundCube.vertices[i].z));
        colors.push_back(backgroundCube.colors[i]);
        Textures.push_back(backgroundCube.texcoord[i]);

    }

    for (size_t i = 0; i < backgroundCube.face_count; i++)
    {
        indices.push_back(backgroundCube.faces[i].v1 - 1 + vertexOffset);
        indices.push_back(backgroundCube.faces[i].v2 - 1 + vertexOffset);
        indices.push_back(backgroundCube.faces[i].v3 - 1 + vertexOffset);

    }
    vertexOffset += backgroundCube.vertex_count;
    normalOffset += backgroundCube.face_count * 3;
    // ��� �Ǹ��� ���� ������Ʈ
    const Model& backgroundCylinder = static_cast<Model>(background);

    for (size_t i = 0; i < backgroundCylinder.vertex_count; i++) 
    {
        vertices.push_back(glm::vec3(backgroundCylinder.vertices[i].x, backgroundCylinder.vertices[i].y, backgroundCylinder.vertices[i].z));
        colors.push_back(backgroundCylinder.colors[i]);
        Textures.push_back(backgroundCylinder.texcoord[i]);
    }
    for (size_t i = 0; i < backgroundCylinder.face_count; i++) 
    {
        indices.push_back(backgroundCylinder.faces[i].v1 - 1 + vertexOffset);
        indices.push_back(backgroundCylinder.faces[i].v2 - 1 + vertexOffset);
        indices.push_back(backgroundCylinder.faces[i].v3 - 1 + vertexOffset);
    }
    
    vertexOffset += backgroundCylinder.vertex_count;
    normalOffset += backgroundCylinder.face_count * 3;

    // ���� ������ü�� ���� ������Ʈ
    for (const auto& cube : background.cube_models) 
    {
        for (size_t i = 0; i < cube.vertex_count; i++) 
        {
            vertices.push_back(glm::vec3(cube.vertices[i].x, cube.vertices[i].y, cube.vertices[i].z));
            colors.push_back(cube.colors[i]);
            Textures.push_back(cube.texcoord[i]);
        }
        for (size_t i = 0; i < cube.face_count; i++) 
        {
            indices.push_back(cube.faces[i].v1 - 1 + vertexOffset);
            indices.push_back(cube.faces[i].v2 - 1 + vertexOffset);
            indices.push_back(cube.faces[i].v3 - 1 + vertexOffset);

        }
        vertexOffset += cube.vertex_count;
        normalOffset += cube.face_count * 3;
    }
   
    
   // objects ������ ��� ��ü ������Ʈ (�÷��̾�, �� ��)
    for (const auto& object : objects)
    {
       
        for (size_t i = 0; i < object->vertex_count; i++)
        {
            const Vertex& vertex = object->vertices[i];
            vertices.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));
            colors.push_back(object->colors[i]);
            Textures.push_back(object->texcoord[i]);
        }
        for (size_t i = 0; i < object->face_count; i++)
        {
            const Face& face = object->faces[i];
            indices.push_back(face.v1 - 1 + vertexOffset);
            indices.push_back(face.v2 - 1 + vertexOffset);
            indices.push_back(face.v3 - 1 + vertexOffset);
        }
        vertexOffset += object->vertex_count;
        normalOffset += object->face_count * 3;
    }

    // �Ѿ� ���� ������Ʈ
    for (const auto& bullet : bulletPool.getAllBullets()) 
    {
        if (bullet.is_active) {
            
            for (size_t i = 0; i < bullet.vertex_count; i++) 
            {
                vertices.push_back(glm::vec3(bullet.vertices[i].x, bullet.vertices[i].y, bullet.vertices[i].z));
                colors.push_back(bullet.colors[i]);
                Textures.push_back(bullet.texcoord[i]);
            }
            for (size_t i = 0; i < bullet.face_count; i++)
            {
                indices.push_back(bullet.faces[i].v1 - 1 + vertexOffset);
                indices.push_back(bullet.faces[i].v2 - 1 + vertexOffset);
                indices.push_back(bullet.faces[i].v3 - 1 + vertexOffset);
            }
            vertexOffset += bullet.vertex_count;
            normalOffset += bullet.face_count * 3;
        }
    }
    
    ///�ﰢ�� ������ ��� ���
    std::vector<glm::vec3> normal(normalOffset, glm::vec3(0.0f));

    for (size_t i = 0; i < indices.size(); i += 3) {
        // �ﰢ���� �����ϴ� �� ������ �ε���
        unsigned int v1 = indices[i];
        unsigned int v2 = indices[i + 1];
        unsigned int v3 = indices[i + 2];

        // �� ������ ��ǥ
        glm::vec3 p1 = vertices[v1];
        glm::vec3 p2 = vertices[v2];
        glm::vec3 p3 = vertices[v3];

        // �ﰢ���� �� �� ���
        glm::vec3 edge1 = p2 - p1;
        glm::vec3 edge2 = p3 - p1;

        // �ﰢ���� �븻 ���� ��� (����)
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        // ���� �븻�� �������� ����
        normal[v1] += faceNormal;
        normal[v2] += faceNormal;
        normal[v3] += faceNormal;
    }
    ///�븻 ����ȭ
    for (size_t i = 0; i < normal.size(); i++) {
        normal[i] = glm::normalize(normal[i]);
    }
    normalinf = normal;


    //���� ��ȭ�� �˱� ���� ��� �ܻ�����
    /*
    size_t temp = vertexOffset;
    colors.clear();
    for (size_t i = 0; i < temp; i++)
    {
        colors.push_back(glm::vec3(1.0f,1.0f,1.0f));
    }
    */
    
   

    // ���� ������ ������Ʈ
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, normalinf.size() * sizeof(glm::vec3), normalinf.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, Textures.size() * sizeof(glm::vec2), Textures.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}
void initShapesBuffer()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(4, vbo);
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

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}
void make_vertexShaders()
{
    GLchar* vertexSource = fileToBuf("vertex.glsl");

    // vertex ���̴� ��ü ���� -> ���̴� �ڵ� ���� -> ������
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);


    // ������ ���� ���� üũ
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader ������ ����" << std::endl << errorLog << std::endl;
        return;
    }
}
void make_fragmentShader()
{
    GLchar* fragmentSource = fileToBuf("fragment.glsl");

    // fragment ���̴� ��ü ���� -> ���̴� �ڵ� ���� -> ������
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // ������ ���� ���� üũ
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader ������ ����" << std::endl << errorLog << std::endl;
        return;
    }
}
GLint make_shaderProgram()
{
    // vertex, fragment ���̴� �����
    make_vertexShaders();
    make_fragmentShader();

    // ���̴� ���α׷�
    shaderProgramID = glCreateProgram();

    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ���� �������� ���� Ȯ��
    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderProgramID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program ���� ����" << std::endl << errorLog << std::endl;
        return false;
    }

    // ���̴� ���α׷� ���
    glUseProgram(shaderProgramID);
}

GLvoid drawScene()
{
    if (drawModeSwitch) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    
    
    glUseProgram(shaderProgramID);
    glBindVertexArray(vao);

    float fov = 45.0f;

    view = glm::lookAt(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(fov), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 200.0f);

    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgramID, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glm::vec3 position = glm::vec3(0.0f, 10.0f, -49.0f);
    float rotation_x = backgroundtexture.rotation_x;

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation_x, glm::vec3(1.0f, 0.0f, 0.0f));

    glm::vec3 rotatedPosition = glm::vec3(rotationMatrix * glm::vec4(position, 1.0f));


    glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), rotatedPosition.x, rotatedPosition.y, rotatedPosition.z);//������ ��ġ
    glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), 0.8f, 0.8f, 0.8f);//���� ��
    glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), 0.0f, 10.0f, 20.0f);//ī�޶� ��ġ
    

    //�޹�� �ؽ���
    draw_backgroundtexture();

    draw_background();
    draw_objects();
    if (draw_bb_switch)
    {
        draw_object_bb();
    }
    glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'w':
        draw_bb_switch = !draw_bb_switch;
        break;
    //case '1':
    //{
    //    Enemy* new_enemy = new Enemy();
    //    float x_pos = rand() % 10 - 5; // ���� x��ǥ -5 ~ 5 ������ ����
    //    new_enemy->init(cylinderModel, x_pos, 0.0f, -50.0f);
    //    objects.push_back(new_enemy);
    //    add_collision_pair("ally_bullet:enemy", nullptr, new_enemy);
    //    add_collision_pair("player:enemy", nullptr, new_enemy);

    //    updateShapeBuffer();
    //    break;
    //}
    //case '2':
    //    slow_switch = !slow_switch;
    //    break;
    case ' ':
    {
        if (!player.is_active)
        {
            return;
        }

        // Bullet ��ü ���� �� �߰�
        Model bulletModel = sphereModel;
        Bullet* newBullet = bulletPool.getBullet(bulletModel, player.position_x, player.position_y, player.position_z, -0.05f);
        newBullet->bb = newBullet->get_bb();
        add_collision_pair("ally_bullet:enemy", newBullet, nullptr);
        
        // �߰� �Ѿ� �߻� ����
        /*Model bulletModel2 = sphereModel;
        Bullet* newBullet2 = bulletPool.getBullet(bulletModel, player.position_x, player.position_y, player.position_z, -0.05f);
        newBullet2->bb = newBullet2->get_bb();
        add_collision_pair("ally_bullet:enemy", newBullet2, nullptr);*/
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
    if (!player.is_active)
    {
        return;
    }

    player.handle_events(SPECIAL_KEYBOARD_KEYDOWN, '0', key, x, y);
    glutPostRedisplay();
}
GLvoid SpecialKeyboardUp(int key, int x, int y)
{
    if (!player.is_active)
    {
        return;
    }

    player.handle_events(SPECIAL_KEYBOARD_KEYUP, '0', key, x, y);
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

    // ���ο��� ȿ�� �����
    if (slow_switch)
    {
        frame_time /= 10;
    }

    // �浹 ó��
    handle_collisions();
    
    // ��� ������Ʈ
    background.update(frame_time);

    backgroundtexture.update(frame_time);
    // ������Ʈ(�÷��̾�, ��) ������Ʈ
    int i = 0;
    for (auto& object : objects)
    {
        object->update(frame_time);
        objects[i] = object;
        i++;
        //std::cout << objects.size() << std::endl;
    }

    if (!player.is_active)
    {
        slow_switch = true;
    }

    // �Ѿ� ������Ʈ
    bulletPool.update(frame_time);

    // objects ���� �ʱ�ȭ (���� �Ѿ� ����)
    objects.erase(
        std::remove_if(objects.begin(), objects.end(),
            [](Object* obj) 
            { 
                return dynamic_cast<Bullet*>(obj) != nullptr; 
            }
        ), objects.end());

    // Ȱ��ȭ�� �Ѿ˵��� objects ���Ϳ� �߰�
    bulletPool.addActiveBulletsToObjects(objects);


    // ������ �ð��� ������ ����Ʈ ���
    //std::cout << "Frame Time: " << frame_time * 1000.0f << " ms, Frame Rate: " << frame_rate << " FPS\n";
    last_update_time = current_time;

    //// �浹 �׷� ���
    //std::cout << "Total Groups: " << collision_pairs.size() << std::endl;
    //for (const auto& pair : collision_pairs)
    //{
    //    const std::string& group = pair.first;
    //    const auto& objects_a = pair.second.first;
    //    const auto& objects_b = pair.second.second;

    //    // �׷� �̸� ���
    //    std::cout << "Group: " << group << std::endl;

    //    // ù ��° ���Ϳ� �ִ� ������Ʈ�� ���
    //    std::cout << "  Objects in Group A:" << std::endl;
    //    for (const auto* obj : objects_a) {
    //        if (obj) {
    //            std::cout << "    Object at: " << obj << ", Active: " << obj->is_active << std::endl;
    //        }
    //    }

    //    // �� ��° ���Ϳ� �ִ� ������Ʈ�� ���
    //    std::cout << "  Objects in Group B:" << std::endl;
    //    for (const auto* obj : objects_b) 
    //    {
    //        if (obj) 
    //        {
    //            std::cout << "    Object at: " << obj << ", Active: " << obj->is_active << std::endl;
    //        }
    //    }
    //}

    objects.erase(std::remove_if(objects.begin(), objects.end(),
        [](Object* obj)
        {
            Enemy* enemy = dynamic_cast<Enemy*>(obj);
            if (enemy && !enemy->is_active)
            {
                remove_collision_object(enemy);
                delete enemy;
                return true;
            }
            return false;
        }), objects.end());

    updateShapeBuffer();
    glutPostRedisplay();
}
GLvoid enemy_wave_manager(int value)
{
    const int ENEMY_PER_WAVE = 5;
    float x_pos;
    float start_x = -5.0f; // ���� x ��ǥ
    float end_x = 5.0f;    // �� x ��ǥ
    float interval = (end_x - start_x) / (ENEMY_PER_WAVE - 1); // ���� ������ ����

    for (int i = 0; i < ENEMY_PER_WAVE; ++i)
    {
        x_pos = start_x + i * interval; // �� ���� x ��ǥ ���
        Enemy* new_enemy = new Enemy();
        new_enemy->init(cylinderModel, x_pos, 0.0f, -50.0f);
        objects.push_back(new_enemy);
        add_collision_pair("ally_bullet:enemy", nullptr, new_enemy);
        add_collision_pair("player:enemy", nullptr, new_enemy);

    }

    int wave_interval = 3000;
    updateShapeBuffer();
    glutTimerFunc(wave_interval, enemy_wave_manager, 0);
}