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
   -1.0f, 0.5f, 0.5f, //앞면
   -1.0f,-0.5f, 0.5f,
    1.0f,-0.5f, 0.5f,
    1.0f, 0.5f, 0.5f,

   -1.0f, 0.5f,-10.5f, //윗면
   -1.0f, 0.5f,  0.5f,
    1.0f, 0.5f,  0.5f,
    1.0f, 0.5f,-10.5f,

   -1.0f, 0.5f,-10.5f, //왼쪽옆
   -1.0f,-0.5f,-10.5f,
   -1.0f,-0.5f,  0.5f,
   -1.0f, 0.5f,  0.5f,

    1.0f, 0.5f,-10.5f, //뒷면
    1.0f,-0.5f,-10.5f,
   -1.0f,-0.5f,-10.5f,
   -1.0f, 0.5f,-10.5f,

   -1.0f,-0.5f,  0.5f, //아랫면
   -1.0f,-0.5f,-10.5f,
    1.0f,-0.5f,-10.5f,
    1.0f,-0.5f,  0.5f,
      
    1.0f, 0.5f,  0.5f, //오른쪽 옆
    1.0f,-0.5f,  0.5f,
    1.0f,-0.5f,-10.5f,
    1.0f, 0.5f,-10.5f

};//정육면체, 축,정사면체 벡터들

float vertexColor[] = {
   0.149,0.275,0.314, // 앞면
   0.149,0.275,0.314,
   0.149,0.275,0.314,
   0.149,0.275,0.314,
       
   0.0078,0.6275,0.5882, // 윗면
   0.0078,0.6275,0.5882,
   0.0078,0.6275,0.5882,
   0.0078,0.6275,0.5882,
       
   0.149,0.275,0.314, // 왼쪽면
   0.149,0.275,0.314,
   0.149,0.275,0.314,
   0.149,0.275,0.314,
       
   0.149,0.275,0.314, // 뒷면
   0.149,0.275,0.314,
   0.149,0.275,0.314,
   0.149,0.275,0.314,
       
   0.149,0.275,0.314, // 아랫면
   0.149,0.275,0.314,
   0.149,0.275,0.314,
   0.149,0.275,0.314,
       
   0.149,0.275,0.314, // 오른쪽면
   0.149,0.275,0.314,
   0.149,0.275,0.314,
   0.149,0.275,0.314,

};//정육면체, 축,정사면체 색깔들

GLchar* vertexSource, * fragmentSource;
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO[2];

bool hFlag = false;
bool yFlag = true;

float yAngle{};

// 구의 초기 위치를 저장하는 변수
glm::vec3 spherePosition(0.0f, 2.0f, 2.5f);

glm::vec3 targetPosition = spherePosition; // 목표 위치
float moveSpeed = 0.02f; // 이동 속도


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
    //--- 버텍스 세이더 객체 만들기
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //--- 세이더 코드를 세이더 객체에 넣기
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    //--- 버텍스 세이더 컴파일하기
    glCompileShader(vertexShader);
    //--- 컴파일이 제대로 되지 않은 경우: 에러 체크
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}

void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragmentShaderSource.glsl");
    //--- 프래그먼트 세이더 객체 만들기
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //--- 세이더 코드를 세이더 객체에 넣기
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    //--- 프래그먼트 세이더 컴파일
    glCompileShader(fragmentShader);
    //--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
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
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
        exit(-1);
    }
    glUseProgram(ShaderProgramID);

    return ShaderProgramID;
}

void InitBuffer()
{
    glGenVertexArrays(1, &VAO); //--- VAO 를 지정하고 할당하기
    glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기


    glBindVertexArray(VAO); //--- VAO를 바인드하기
    //--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    //--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
    //--- triShape 배열의 사이즈: 9 * float
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);


    //--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute 인덱스 0번을 사용가능하게 함
    glEnableVertexAttribArray(0);
    //--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    //--- 변수 colors에서 버텍스 색상을 복사한다.
    //--- colors 배열의 사이즈: 9 *float 
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexColor), vertexColor, GL_STATIC_DRAW);
    //--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute 인덱스 1번을 사용 가능하게 함.
    glEnableVertexAttribArray(1);

}

GLvoid drawScene() {

    glUseProgram(shaderID);
    glBindVertexArray(VAO);

    glClearColor(0.698f, 0.886f, 0.871f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    hFlag ? glDisable(GL_DEPTH_TEST) : glEnable(GL_DEPTH_TEST);//은면제거
    yFlag ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//와이어 객체

    unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//월드 변환 행렬값을 셰이더의 uniform mat4 modelTransform에게 넘겨줌
    unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//위와 동일
    unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//위와 동일
    unsigned int isSphereLocation = glGetUniformLocation(shaderID, "isSphere");
    unsigned int sphereColorLocation = glGetUniformLocation(shaderID, "sphereColor");

    //원근 투영
    glm::mat4 kTransform = glm::mat4(1.0f);
    kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -5.0f));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

    //뷰잉 변환
    glm::mat4 vTransform = glm::mat4(1.0f);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.7f, 1.0f); //--- 카메라 위치
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

    vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);


    // 맵 생성
    glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
    shapeTransForm = glm::rotate(shapeTransForm, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0));//y축 회전
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm));//변환 행렬을 셰이더에 전달
    glUniform1i(isSphereLocation, 0); // 직육면체일 때 isSphere를 0으로 설정
    glDrawArrays(GL_QUADS, 0, 24); //정육면체


    // 구를 위한 변환 행렬
    glm::mat4 sphereModel = glm::mat4(1.0f);
    sphereModel = glm::translate(sphereModel, spherePosition); // 구의 위치를 변수로 설정
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(sphereModel));
    glUniform1i(isSphereLocation, 1); // 구일 때, isSphere을 1로 설정
    glUniform3f(sphereColorLocation, 1.0f, 1.0f, 1.0f); // 구 색상


    // GLU 구 생성 및 그리기
    GLUquadricObj* qobj;
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, GLU_FILL); // 스타일(와이어LINE, 면FILL)
    gluQuadricNormals(qobj, GLU_SMOOTH); // 부드러운 노멀
    gluQuadricOrientation(qobj, GLU_OUTSIDE); // 외부 방향 설정(이러면 카메라가 구 밖에서 구 표면을 보게 됨)
    gluSphere(qobj, 0.05, 50, 50); // 반지름 10.05, 50개의 세그먼트와 스택


    glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

GLvoid KeyBoard(unsigned char key, int x, int y) {

    switch (key) {
    case 'h':
        hFlag = 1 - hFlag; // 토글 방식
        break;
    case 'y':
        yFlag = !yFlag; // 토글 방식
        break;
    case 'w':
        targetPosition.z -= 0.5f; // 목표 위치 업데이트
        break;
    case 'a':
        targetPosition.x -= 0.5f; // 목표 위치 업데이트
        break;
    case 's':
        targetPosition.z += 0.5f; // 목표 위치 업데이트
        break;
    case 'd':
        targetPosition.x += 0.5f; // 목표 위치 업데이트
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
    // 공이 목표 위치에 도달하지 않았으면 보간 처리
    if (glm::distance(spherePosition, targetPosition) > 0.01f) {
        glm::vec3 direction = glm::normalize(targetPosition - spherePosition);
        spherePosition += direction * moveSpeed;

        // 목표 위치를 넘어가지 않도록 클램핑
        if (glm::distance(spherePosition, targetPosition) < moveSpeed) {
            spherePosition = targetPosition;
        }
    }

    glutTimerFunc(10, TimerFunc, 1); // 타이머 설정
    glutPostRedisplay(); // 화면 갱신
}