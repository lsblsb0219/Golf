#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <Windows.h>
#include <time.h>

GLvoid drawScene();
GLvoid KeyBoard(unsigned char key, int x, int y);
GLvoid SpecialKeyBoard(int key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid TimerFunc(int x);

void InitBuffer();
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
char* filetobuf(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb"); // Open file for reading 
    if (!fptr) // Return NULL on failure 
        return NULL;
    fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
    length = ftell(fptr); // Find out how many bytes into the file we are 
    buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
    fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
    fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
    fclose(fptr); // Close the file 
    buf[length] = 0; // Null terminator 
    return buf; // Return the buffer 
}

float vertexPosition[] = {
   -1.0f, 0.5f, 0.5f, //�ո�
   -1.0f,-0.5f, 0.5f,
    1.0f,-0.5f, 0.5f,
    1.0f, 0.5f, 0.5f,

   -1.0f, 0.5f,-10.5f, //����
   -1.0f, 0.5f,  0.5f,
    1.0f, 0.5f,  0.5f,
    1.0f, 0.5f,-10.5f,

   -1.0f, 0.5f,-10.5f, //���ʿ�
   -1.0f,-0.5f,-10.5f,
   -1.0f,-0.5f,  0.5f,
   -1.0f, 0.5f,  0.5f,

    1.0f, 0.5f,-10.5f, //�޸�
    1.0f,-0.5f,-10.5f,
   -1.0f,-0.5f,-10.5f,
   -1.0f, 0.5f,-10.5f,

   -1.0f,-0.5f,  0.5f, //�Ʒ���
   -1.0f,-0.5f,-10.5f,
    1.0f,-0.5f,-10.5f,
    1.0f,-0.5f,  0.5f,
      
    1.0f, 0.5f,  0.5f, //������ ��
    1.0f,-0.5f,  0.5f,
    1.0f,-0.5f,-10.5f,
    1.0f, 0.5f,-10.5f

};//������ü, ��,�����ü ���͵�

float vertexColor[] = {
   0.149,0.275,0.314, // �ո�
   0.149,0.275,0.314,
   0.149,0.275,0.314,
   0.149,0.275,0.314,
       
   0.0078,0.6275,0.5882, // ����
   0.0078,0.6275,0.5882,
   0.0078,0.6275,0.5882,
   0.0078,0.6275,0.5882,
       
   0.149,0.275,0.314, // ���ʸ�
   0.149,0.275,0.314,
   0.149,0.275,0.314,
   0.149,0.275,0.314,
       
   0.149,0.275,0.314, // �޸�
   0.149,0.275,0.314,
   0.149,0.275,0.314,
   0.149,0.275,0.314,
       
   0.149,0.275,0.314, // �Ʒ���
   0.149,0.275,0.314,
   0.149,0.275,0.314,
   0.149,0.275,0.314,
       
   0.149,0.275,0.314, // �����ʸ�
   0.149,0.275,0.314,
   0.149,0.275,0.314,
   0.149,0.275,0.314,

};//������ü, ��,�����ü �����

GLchar* vertexSource, * fragmentSource;
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;

int main(int argc, char** argv)
{
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("GolfGame");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";

    make_vertexShaders();
    make_fragmentShaders();
    shaderID = make_shaderProgram();
    InitBuffer();
    glutKeyboardFunc(KeyBoard);
    glutSpecialFunc(SpecialKeyBoard);
    glutTimerFunc(10, TimerFunc, 1);
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMainLoop();
}
void make_vertexShaders()
{
    vertexSource = filetobuf("vertexShaderSource.glsl");
    //--- ���ؽ� ���̴� ��ü �����
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    //--- ���ؽ� ���̴� �������ϱ�
    glCompileShader(vertexShader);
    //--- �������� ����� ���� ���� ���: ���� üũ
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
        return;
    }
}
void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragmentShaderSource.glsl");
    //--- �����׸�Ʈ ���̴� ��ü �����
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    //--- �����׸�Ʈ ���̴� ������
    glCompileShader(fragmentShader);
    //--- �������� ����� ���� ���� ���: ������ ���� üũ
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
        return;
    }
}
GLuint make_shaderProgram()
{
    GLuint ShaderProgramID;

    ShaderProgramID = glCreateProgram();
    glAttachShader(ShaderProgramID, vertexShader);
    glAttachShader(ShaderProgramID, fragmentShader);
    glLinkProgram(ShaderProgramID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];

    glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
        exit(-1);
    }
    glUseProgram(ShaderProgramID);

    return ShaderProgramID;
}
bool depth = false;
GLuint VAO, VBO[2];
void InitBuffer()
{
    glGenVertexArrays(1, &VAO); //--- VAO �� �����ϰ� �Ҵ��ϱ�
    glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�


    glBindVertexArray(VAO); //--- VAO�� ���ε��ϱ�
    //--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    //--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
    //--- triShape �迭�� ������: 9 * float
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);


    //--- ��ǥ���� attribute �ε��� 0���� �����Ѵ�: ���ؽ� �� 3* float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute �ε��� 0���� ��밡���ϰ� ��
    glEnableVertexAttribArray(0);
    //--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    //--- ���� colors���� ���ؽ� ������ �����Ѵ�.
    //--- colors �迭�� ������: 9 *float 
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexColor), vertexColor, GL_STATIC_DRAW);
    //--- ������ attribute �ε��� 1���� �����Ѵ�: ���ؽ� �� 3*float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute �ε��� 1���� ��� �����ϰ� ��.
    glEnableVertexAttribArray(1);

}

bool hFlag = false;
bool wFlag = true;
bool cFlag = false;

bool xFlag = false;
float xAngle{};

bool yFlag = false;
float yAngle{};

float xTrans{}, yTrans{};

GLvoid drawScene() {

    glUseProgram(shaderID);
    glBindVertexArray(VAO);

    glClearColor(0.698f, 0.886f, 0.871f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    hFlag ? glDisable(GL_DEPTH_TEST) : glEnable(GL_DEPTH_TEST);//��������
    wFlag ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//���̾� ��ü

    unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//���� ��ȯ ��İ��� ���̴��� uniform mat4 modelTransform���� �Ѱ���
    unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//���� ����
    unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//���� ����

    //���� ����
    glm::mat4 kTransform = glm::mat4(1.0f);
    kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -5.0f));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

    //���� ��ȯ
    glm::mat4 vTransform = glm::mat4(1.0f);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.7f, 1.0f); //--- ī�޶� ��ġ
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����

    vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

    glm::mat4 shapeTransForm = glm::mat4(1.0f);//��ȯ ��� ���� T
    shapeTransForm = glm::translate(shapeTransForm, glm::vec3(xTrans, yTrans, 0.0));//�̵�
    shapeTransForm = glm::rotate(shapeTransForm, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0));//y�� ȸ��
    shapeTransForm = glm::rotate(shapeTransForm, glm::radians(xAngle), glm::vec3(1.0, 0.0, 0.0));//x�� ȸ��
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm));//��ȯ ����� ���̴��� ����
    

    glDrawArrays(GL_QUADS, 0, 24); //������ü

    glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

GLvoid KeyBoard(unsigned char key, int x, int y) {

    switch (key) {
    case 'h':
        hFlag = 1 - hFlag;//��� ���
        break;
    case 'w':
        wFlag = !wFlag;//��� ���
        break;
    default:
        exit(-1);

    }
    glutPostRedisplay();
}

GLvoid SpecialKeyBoard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        yAngle = yAngle - 1.f;
        if (yAngle > 360)yAngle = 0.f;
        break;
    case GLUT_KEY_RIGHT:
        yAngle = yAngle + 1.f;
        if (yAngle > 360)yAngle = 0.f;
        break;
    }
    glutPostRedisplay();
}

GLvoid TimerFunc(int x) {
    switch (x)
    {
    case 1:        
        break;
    }

    glutTimerFunc(10, TimerFunc, 1);
    glutPostRedisplay();
}