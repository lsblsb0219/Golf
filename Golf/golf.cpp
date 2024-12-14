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
#include <sstream>

GLvoid drawScene();
GLvoid KeyBoard(unsigned char key, int x, int y);
GLvoid SpecialKeyBoard(int key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid TimerFunc(int x);

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

// ------- 기타 선언 함수 --------
void InitBuffer();
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void checkCollision();
void restrictTargetPosition();
void resetBallPosition();
void updateObstaclePosition();
void checkObstacleCollision();
void drawRectangle(float x, float y, float width, float height, float r, float g, float b);


// ------- OpenGL 객체 --------
GLchar* vertexSource, * fragmentSource;
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO, VBO[2];

bool hFlag = false;
bool yFlag = true;

float yAngle{};

// -------- 공 --------
glm::vec3 spherePosition(0.0f, 0.55f, 0.0f); // 구의 초기 위치를 저장하는 변수

glm::vec3 targetPosition = spherePosition; // 목표 위치
float moveSpeed = 0.02f; // 이동 속도

// 애니메이션 진행 상태를 나타내는 플래그
bool isAnimating = false;

// -------- 카메라 --------
glm::vec3 cameraOffset(0.0f, 0.3f, 0.5f); // 공과 카메라 사이의 고정 거리 (위, 뒤)
glm::vec3 cameraPos = spherePosition + cameraOffset; // 초기 카메라 위치
glm::vec3 initialCameraDir = glm::normalize(-cameraOffset); // 공을 바라보는 초기 방향

// -------- 골대 --------
glm::mat4 GoalTransForm;

float GoalLocationX = 0.0f;
float GoalLocationY = 0.0f;
float GoalLocationZ = 0.0f;

bool Stage3State = false; // 깃대 움직임 상태

// -------- 장애물 --------
glm::vec3 obstacle1Position(2.32f, 0.55f, -10.0f); // 장애물 1의 초기 위치
glm::vec3 obstacle2Position(0.85f, 0.55f, -3.0f); // 장애물 2의 초기 위치
float obstacleMoveSpeed = 0.02f; // 장애물 이동 속도
float obstacleMoveRange = 1.0f; // 장애물 이동 범위
bool obstacle1MovingRight = true; // 장애물 1 이동 방향
bool obstacle2MovingRight = false; // 장애물 2 이동 방향


// -------- 충돌 --------
bool isCollisionDetected1 = false; // 충돌 상태 추적 변수
bool isCollisionDetected2 = false;
bool isCollisionDetected3 = false;
bool isCollisionDetected4 = false;

// 스테이지별 경계값
float stage1BoundaryMinX = -0.95f, stage1BoundaryMaxX = 0.95f;
float stage1BoundaryMinZ = -10.45f, stage1BoundaryMaxZ = 0.45f;

float stage2BoundaryMinX = 1.05f, stage2BoundaryMaxX = 2.95f;
float stage2BoundaryMinZ = -15.45f, stage2BoundaryMaxZ = -4.55f;

float boundaryY = 0.55f;  // Y축 고정값 (육면체 상단)

// AABB 정의 (구체를 감싸는 AABB)
struct AABB {
	glm::vec3 min;  // 최소점
	glm::vec3 max;  // 최대점
};
// 충돌 박스 변수
glm::vec3 goalBoxMin, goalBoxMax;
// 깃대의 충돌박스를 계산하는 함수
bool checkAABBCollision(const AABB& a, const AABB& b);
AABB createGolfBallAABB(const glm::vec3& center, float radius);
void checkCollision();

// -------- 맵 --------
int currentMapStage = 3; // 현재 맵 스테이지

// 이동 거리
float move_len = 1.0f;

int state = 0;


// -------- 글씨 --------
void renderBitmapString(float x, float y, void* font, const char* string);


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


	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);



	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexColor), vertexColor, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

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

	// 뷰잉 변환
	glm::mat4 vTransform = glm::mat4(1.0f);
	// 카메라가 공을 바라보도록 설정
	glm::vec3 cameraDirection = spherePosition; // 공의 현재 위치를 바라봄
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // 카메라의 위쪽 방향

	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

	// 맵 생성
	if (currentMapStage == 1) {
		glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
		shapeTransForm = glm::rotate(shapeTransForm, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0));//y축 회전
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm));//변환 행렬을 셰이더에 전달
		glUniform1i(isSphereLocation, 0); // 직육면체일 때 isSphere를 0으로 설정
		glDrawArrays(GL_QUADS, 0, 24); //정육면체

		// 깃대 생성
		GoalLocationX = 0.0f;
		GoalLocationY = 0.5f;
		GoalLocationZ = -10.0f;

		GoalTransForm = glm::mat4(1.0f);
		GoalTransForm = glm::translate(GoalTransForm, glm::vec3(GoalLocationX, GoalLocationY, GoalLocationZ));
		GoalTransForm = glm::scale(GoalTransForm, glm::vec3(0.05f, 2.0f, 0.01f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(GoalTransForm));
		glUniform1i(isSphereLocation, 0); // 직육면체일 때 isSphere를 0으로 설정
		glDrawArrays(GL_QUADS, 0, 24);
	}
	else if (currentMapStage == 2 || currentMapStage == 3) {
		// 첫 번째 정육면체
		glm::mat4 shapeTransForm = glm::mat4(1.0f); // 기본 변환 행렬 생성
		shapeTransForm = glm::rotate(shapeTransForm, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0)); // y축 회전
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm)); // 변환 행렬을 셰이더에 전달
		glUniform1i(isSphereLocation, 0); // 직육면체일 때 isSphere를 0으로 설정
		glDrawArrays(GL_QUADS, 0, 24); // 정육면체

		// 두 번째 정육면체 (오른쪽에 배치)
		glm::mat4 shapeTransForm2 = glm::mat4(1.0f); // 기본 변환 행렬 생성
		shapeTransForm2 = glm::rotate(shapeTransForm2, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0)); // y축 회전
		shapeTransForm2 = glm::translate(shapeTransForm2, glm::vec3(2.0f, 0.0f, -5.0f)); // 오른쪽으로 이동
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm2)); // 변환 행렬을 셰이더에 전달
		glUniform1i(isSphereLocation, 0); // 직육면체일 때 isSphere를 0으로 설정
		glDrawArrays(GL_QUADS, 0, 24); // 정육면체

		// 깃대 생성
		if ((currentMapStage == 3 && !Stage3State) || (currentMapStage == 2)) {
			GoalLocationX = 2.0f;
			GoalLocationY = 0.5f;
			GoalLocationZ = -15.0f;
		}

		GoalTransForm = glm::mat4(1.0f);
		GoalTransForm = glm::translate(GoalTransForm, glm::vec3(GoalLocationX, GoalLocationY, GoalLocationZ));
		GoalTransForm = glm::scale(GoalTransForm, glm::vec3(0.05f, 2.0f, 0.01f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(GoalTransForm));
		glUniform1i(isSphereLocation, 0); // 직육면체일 때 isSphere를 0으로 설정
		glDrawArrays(GL_QUADS, 0, 24);

		// 장애물 1 렌더링
		glm::mat4 obstacle1Transform = glm::mat4(1.0f);
		obstacle1Transform = glm::translate(obstacle1Transform, obstacle1Position);
		obstacle1Transform = glm::scale(obstacle1Transform, glm::vec3(0.2f, 0.5f, 0.1f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(obstacle1Transform));
		glUniform1i(isSphereLocation, 0); // 직육면체로 설정
		glDrawArrays(GL_QUADS, 0, 24);

		// 장애물 2 렌더링
		glm::mat4 obstacle2Transform = glm::mat4(1.0f);
		obstacle2Transform = glm::translate(obstacle2Transform, obstacle2Position);
		obstacle2Transform = glm::scale(obstacle2Transform, glm::vec3(0.3f, 0.5f, 0.07f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(obstacle2Transform));
		glUniform1i(isSphereLocation, 0); // 직육면체로 설정
		glDrawArrays(GL_QUADS, 0, 24);

		if (currentMapStage == 3) {
			if (Stage3State || spherePosition.z <= -13.0f) {
				GoalLocationX = 0.0f;
				GoalLocationY = 0.5f;
				GoalLocationZ = 0.0f;

				Stage3State = true; // 깃대 이동했다는 상태
			}
		}
	}
	else if (currentMapStage == 4) {
		// 첫 번째 사각형
		glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
		shapeTransForm = glm::rotate(shapeTransForm, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0));//y축 회전
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm));//변환 행렬을 셰이더에 전달
		glUniform1i(isSphereLocation, 0); // 직육면체일 때 isSphere를 0으로 설정
		glDrawArrays(GL_QUADS, 0, 24); //정육면체

		// 두 번째 사각형(절벽 아래)
		glm::mat4 shapeTransForm2 = glm::mat4(1.0f); // 기본 변환 행렬 생성
		shapeTransForm2 = glm::rotate(shapeTransForm2, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0)); // y축 회전
		shapeTransForm2 = glm::translate(shapeTransForm2, glm::vec3(0.0f, -2.0f, -10.0f)); // 아래쪽으로 이동
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm2)); // 변환 행렬을 셰이더에 전달
		glUniform1i(isSphereLocation, 0); // 직육면체일 때 isSphere를 0으로 설정
		glDrawArrays(GL_QUADS, 0, 24); // 정육면체

		// 깃대 생성
		GoalLocationX = 0.0f;
		GoalLocationY = -1.5f;
		GoalLocationZ = -20.0f;

		GoalTransForm = glm::mat4(1.0f);
		GoalTransForm = glm::translate(GoalTransForm, glm::vec3(GoalLocationX, GoalLocationY, GoalLocationZ));
		GoalTransForm = glm::scale(GoalTransForm, glm::vec3(0.05f, 2.0f, 0.01f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(GoalTransForm));
		glUniform1i(isSphereLocation, 0); // 직육면체일 때 isSphere를 0으로 설정
		glDrawArrays(GL_QUADS, 0, 24);

		// 가짜 깃대 생성
		glm::mat4 FakeGoalTransForm = glm::mat4(1.0f);
		FakeGoalTransForm = glm::translate(FakeGoalTransForm, glm::vec3(0.0f, 0.5f, -10.0f));
		FakeGoalTransForm = glm::scale(FakeGoalTransForm, glm::vec3(0.05f, 2.0f, 0.01f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(FakeGoalTransForm));
		glUniform1i(isSphereLocation, 0); // 직육면체일 때 isSphere를 0으로 설정
		glDrawArrays(GL_QUADS, 0, 24);

	}


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
	gluSphere(qobj, 0.05, 50, 50); // 반지름 0.05, 50개의 세그먼트와 스택

	renderBitmapString(0.5f, 0.8f, GLUT_BITMAP_HELVETICA_18, "key");
	renderBitmapString(0.5f, 0.7f, GLUT_BITMAP_HELVETICA_18, "current");
	// 사각형 1: move_len
	float rectHeight1 = move_len * 0.5f; // 이동 거리에 비례
	drawRectangle(1.05f, 1.0f, 0.05f, rectHeight1, 1.0f, 0.0f, 0.0f); // 빨간색
	// 텍스트 출력 (화면 위치, 글꼴, 텍스트 내용)
	renderBitmapString(0.9f, 0.9f, GLUT_BITMAP_HELVETICA_18, "distance");
	renderBitmapString(1.0f, 0.8f, GLUT_BITMAP_HELVETICA_18, "1~4");
	std::ostringstream oss1;
	oss1 << move_len;  // move_len 값을 텍스트로 변환
	renderBitmapString(1.01f, 0.7f, GLUT_BITMAP_HELVETICA_18, oss1.str().c_str());

	// 사각형 2: moveSpeed
	float rectHeight2 = moveSpeed * 10.0f; // 속도에 비례
	drawRectangle(1.35f, 1.0f, 0.05f, rectHeight2, 0.0f, 0.0f, 1.0f); // 파란색
	// 텍스트 출력 (화면 위치, 글꼴, 텍스트 내용)
	renderBitmapString(1.3f, 0.9f, GLUT_BITMAP_HELVETICA_18, "speed");
	renderBitmapString(1.3f, 0.8f, GLUT_BITMAP_HELVETICA_18, "updown");
	std::ostringstream oss2;
	oss2 << moveSpeed;  // moveSpeed 값을 텍스트로 변환
	renderBitmapString(1.3f, 0.7f, GLUT_BITMAP_HELVETICA_18, oss2.str().c_str());

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid KeyBoard(unsigned char key, int x, int y) {
	if (isAnimating) return; // 애니메이션 중이면 입력 무시

	switch (key) {
	case 'h':
		hFlag = 1 - hFlag; // 토글 방식
		break;
	case 'y':
		yFlag = !yFlag; // 토글 방식
		break;
	case '1':
		move_len = 0.5f;
		break;
	case '2':
		move_len = 1.0f;
		break;
	case '3':
		move_len = 1.5f;
		break;
	case '4':
		move_len = 2.0f;
		break;
	case 'W':
	case 'w':
		targetPosition.z -= move_len; // 목표 위치 갱신
		break;
	case 'A':
	case 'a':
		targetPosition.x -= move_len; // 목표 위치 갱신
		break;
	case 'S':
	case 's':
		targetPosition.z += move_len; // 목표 위치 갱신
		break;
	case 'D':
	case 'd':
		targetPosition.x += move_len; // 목표 위치 갱신
		break;
	case 'R':
	case 'r':
		resetBallPosition();

		if (currentMapStage == 3) {
			Stage3State = false;
		}
		break;
	case 'p':
		printf("x : %.2f, y : %.2f, z : %.2f, state = %d\n", targetPosition.x, targetPosition.y, targetPosition.z, state);
		break;
	case 'Q':
	case 'q':
		exit(-1);
	}

	// 스테이지별 경계 제한
	restrictTargetPosition();

	targetPosition.y = boundaryY; // Y축 고정

	// 목표 위치와 현재 위치가 동일하면 애니메이션 종료
	if (glm::distance(spherePosition, targetPosition) < 0.01f) {
		isAnimating = false;
	}
	else {
		isAnimating = true; // 애니메이션 시작
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
	case GLUT_KEY_UP:
		moveSpeed += 0.05f;
		break;
	case GLUT_KEY_DOWN:
		if (moveSpeed - 0.05f >= 0.0f) {
			moveSpeed -= 0.05f;
		}
		break;
	}
	glutPostRedisplay();
}

GLvoid TimerFunc(int x) {
	// 공이 목표 위치에 도달하지 않았으면 보간 처리
	if (glm::distance(spherePosition, targetPosition) > 0.01f) {
		glm::vec3 direction = glm::normalize(targetPosition - spherePosition);
		spherePosition += direction * moveSpeed;

		// 경계 조건 적용
		restrictTargetPosition();
		spherePosition.y = boundaryY; // Y축 고정

		// 목표 위치를 넘어가지 않도록 클램핑
		if (glm::distance(spherePosition, targetPosition) < moveSpeed) {
			spherePosition = targetPosition;
			isAnimating = false; // 애니메이션 종료, 입력 잠금 해제
		}
	}

	// 카메라도 공을 따라 이동
	cameraPos = spherePosition + cameraOffset;

	// 장애물 위치 업데이트
	updateObstaclePosition();

	// 장애물과 충돌 검사
	checkObstacleCollision();

	// 충돌 검사 함수 호출
	checkCollision();

	glutTimerFunc(10, TimerFunc, 1); // 타이머 설정
	glutPostRedisplay(); // 화면 갱신
}

// 깃대 AABB
void updateGoalBoundingBox(glm::mat4 GoalTransForm) {
	// 스케일 정보를 추출하여 충돌 박스 크기 계산
	glm::vec3 scale = glm::vec3(GoalTransForm[0][0], GoalTransForm[1][1], GoalTransForm[2][2]);

	// 이동 정보를 추출하여 충돌 박스 위치 계산
	glm::vec3 position = glm::vec3(GoalTransForm[3][0], GoalTransForm[3][1], GoalTransForm[3][2]);

	// 충돌 박스 최소, 최대 값 계산
	goalBoxMin = position - scale / 2.0f;  // 크기의 절반을 빼면 최소점
	goalBoxMax = position + scale / 2.0f;  // 크기의 절반을 더하면 최대점
}

// 두 AABB 간 충돌을 검사하는 함수
bool checkAABBCollision(const AABB& a, const AABB& b) {
	if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
	if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
	if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
	return true;
}

// 골프공의 AABB 생성
AABB createGolfBallAABB(const glm::vec3& center, float radius) {
	AABB aabb;
	aabb.min = center - glm::vec3(radius, radius, radius);
	aabb.max = center + glm::vec3(radius, radius, radius);
	return aabb;
}

void checkCollision() {
	if (currentMapStage == 1 && isCollisionDetected1) {
		return; // 충돌이 이미 발생했으면 추가적인 충돌 처리 안 함
	}
	else if (currentMapStage == 2 && isCollisionDetected2) {
		return; // 충돌이 이미 발생했으면 추가적인 충돌 처리 안 함
	}
	else if (currentMapStage == 3 && isCollisionDetected3) {
		return; // 충돌이 이미 발생했으면 추가적인 충돌 처리 안 함
	}
	else if (currentMapStage == 4 && isCollisionDetected4) {
		return; // 충돌이 이미 발생했으면 추가적인 충돌 처리 안 함
	}

	// 골프공 AABB 생성 (현재 위치를 사용)
	float golfBallRadius = 0.05f; // 골프공의 반지름
	AABB golfBallAABB = createGolfBallAABB(spherePosition, golfBallRadius);

	// 깃대 AABB 갱신
	updateGoalBoundingBox(GoalTransForm); // GoalTransform을 기반으로 goalBoxMin, goalBoxMax 업데이트
	AABB goalAABB = { goalBoxMin, goalBoxMax };

	// 충돌 검사
	if (checkAABBCollision(golfBallAABB, goalAABB)) {
		std::cout << "충돌 발생!" << std::endl;
		if (currentMapStage == 1) {
			isCollisionDetected1 = true;
			resetBallPosition();
		}
		else if (currentMapStage == 2) {
			isCollisionDetected2 = true;
			resetBallPosition();
		}
		else if (currentMapStage == 3) {
			isCollisionDetected3 = true;
			resetBallPosition();
		}
		else if (currentMapStage == 4) {
			isCollisionDetected4 = true;
			resetBallPosition();
		}

		currentMapStage++;

		if (currentMapStage > 4) {
			currentMapStage = 1; // 마지막 스테이지 이후 처음으로 돌아감
		}
	}
}

// 목표 위치 경계 제한 함수
void restrictTargetPosition() {
	if (currentMapStage == 1) {
		if (targetPosition.x < stage1BoundaryMinX) targetPosition.x = stage1BoundaryMinX;
		if (targetPosition.x > stage1BoundaryMaxX) targetPosition.x = stage1BoundaryMaxX;
		if (targetPosition.z < stage1BoundaryMinZ) targetPosition.z = stage1BoundaryMinZ;
		if (targetPosition.z > stage1BoundaryMaxZ) targetPosition.z = stage1BoundaryMaxZ;
	}
	else if (currentMapStage == 2) {
		if (targetPosition.z >= -4.5f) {
			if (targetPosition.x < stage1BoundaryMinX) targetPosition.x = stage1BoundaryMinX;
			if (targetPosition.x > stage1BoundaryMaxX) targetPosition.x = stage1BoundaryMaxX;
			if (targetPosition.z > stage1BoundaryMaxZ) targetPosition.z = stage1BoundaryMaxZ;
			state = 1;
		}
		else if (targetPosition.x < 1.0f && targetPosition.z >= -10.5f){
			if (targetPosition.x < 2.0f && targetPosition.z >= -15.0f){
				if (targetPosition.x < stage1BoundaryMinX) targetPosition.x = stage1BoundaryMinX;
				if (targetPosition.z < stage1BoundaryMinZ) targetPosition.z = stage1BoundaryMinZ;
				state = 2;
			}
		}
		else if (targetPosition.x >= 1.0f && targetPosition.z >= -10.5f) {
			if (targetPosition.z >= -15.0f){
				if (targetPosition.x > stage2BoundaryMaxX) targetPosition.x = stage2BoundaryMaxX;
				if (targetPosition.z > stage2BoundaryMaxZ) targetPosition.z = stage2BoundaryMaxZ;
				state = 3;
			}
		}
		else if (targetPosition.z >= -20.0f) {
			if (targetPosition.x < stage2BoundaryMinX) targetPosition.x = stage2BoundaryMinX;
			if (targetPosition.x > stage2BoundaryMaxX) targetPosition.x = stage2BoundaryMaxX;
			if (targetPosition.z < stage2BoundaryMinZ) targetPosition.z = stage2BoundaryMinZ;
			state = 4;
		}
		else {
			state = 0;
		}
	}
}

void resetBallPosition() {
	spherePosition = glm::vec3(0.0f, 0.55f, 0.0f);
	targetPosition = spherePosition;
	cameraPos = spherePosition + cameraOffset;
	isAnimating = false;
}

// 장애물 위치 업데이트
void updateObstaclePosition() {
	// 장애물 1 좌우 이동
	if (obstacle1MovingRight) {
		obstacle1Position.x += obstacleMoveSpeed;
		if (obstacle1Position.x > 2.0f + obstacleMoveRange) obstacle1MovingRight = false;
	}
	else {
		obstacle1Position.x -= obstacleMoveSpeed;
		if (obstacle1Position.x < 2.0f - obstacleMoveRange) obstacle1MovingRight = true;
	}

	// 장애물 2 좌우 이동
	if (obstacle2MovingRight) {
		obstacle2Position.x += obstacleMoveSpeed;
		if (obstacle2Position.x > 0.0f + obstacleMoveRange) obstacle2MovingRight = false;
	}
	else {
		obstacle2Position.x -= obstacleMoveSpeed;
		if (obstacle2Position.x < 0.0f - obstacleMoveRange) obstacle2MovingRight = true;
	}
}

// 장애물과 충돌 검사
void checkObstacleCollision() {
	// 장애물 AABB 생성
	AABB obstacle1AABB = createGolfBallAABB(obstacle1Position, 0.2f);
	AABB obstacle2AABB = createGolfBallAABB(obstacle2Position, 0.2f);

	// 공의 AABB 생성
	float golfBallRadius = 0.05f;
	AABB golfBallAABB = createGolfBallAABB(spherePosition, golfBallRadius);

	// 장애물 1과 충돌 검사
	if (checkAABBCollision(golfBallAABB, obstacle1AABB)) {
		spherePosition.z += move_len - 0.2f; // 공을 뒤로 밀기
		targetPosition = spherePosition; // 목표 위치 갱신
		isAnimating = false;
	}

	// 장애물 2와 충돌 검사
	if (checkAABBCollision(golfBallAABB, obstacle2AABB)) {
		spherePosition.z += move_len - 0.2f; // 공을 뒤로 밀기
		targetPosition = spherePosition; // 목표 위치 갱신
		isAnimating = false;
	}
}

// 사각형을 그리는 함수
void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // 화면 좌표로 설정

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(r, g, b); // 색상 설정
	glBegin(GL_QUADS);
	glVertex2f(x, y);                     // 좌하단
	glVertex2f(x + width, y);            // 우하단
	glVertex2f(x + width, y + height/2);   // 우상단
	glVertex2f(x, y + height/2);           // 좌상단
	glEnd();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// 텍스트를 그리는 함수
void renderBitmapString(float x, float y, void* font, const char* string) {
	glRasterPos2f(x, y);  // 텍스트 위치 설정
	for (const char* c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);  // 각 문자 출력
	}
}