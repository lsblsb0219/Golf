#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
GLvoid TimerUpdate(int x);

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

float texCoords[] = {
	// �ո�
	0.0f, 1.0f, // �»�
	0.0f, 0.0f, // ����
	1.0f, 0.0f, // ����
	1.0f, 1.0f, // ���

	// ����
	0.0f, 1.0f, // �»�
	0.0f, 0.0f, // ����
	1.0f, 0.0f, // ����
	1.0f, 1.0f, // ���

	// ���� ����
	0.0f, 1.0f, // �»�
	0.0f, 0.0f, // ����
	1.0f, 0.0f, // ����
	1.0f, 1.0f, // ���

	// �޸�
	0.0f, 1.0f, // �»�
	0.0f, 0.0f, // ����
	1.0f, 0.0f, // ����
	1.0f, 1.0f, // ���

	// �Ʒ���
	0.0f, 1.0f, // �»�
	0.0f, 0.0f, // ����
	1.0f, 0.0f, // ����
	1.0f, 1.0f, // ���

	// ������ ����
	0.0f, 1.0f, // �»�
	0.0f, 0.0f, // ����
	1.0f, 0.0f, // ����
	1.0f, 1.0f  // ���
};

// ------- ��Ÿ ���� �Լ� --------
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
GLuint LoadTexture(const char* filename);

// ------- OpenGL ��ü --------
GLchar* vertexSource, * fragmentSource;
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO, VBO[3], textureID[10];

bool hFlag = true;
bool yFlag = true;

float yAngle{};

// -------- �� --------
glm::vec3 spherePosition(0.0f, 0.55f, 0.0f); // ���� �ʱ� ��ġ�� �����ϴ� ����

glm::vec3 targetPosition = spherePosition; // ��ǥ ��ġ
float moveSpeed = 0.02f; // �̵� �ӵ�

// �̵� �Ÿ�
float move_len = 1.0f;

// �ִϸ��̼� ���� ���¸� ��Ÿ���� �÷���
bool isAnimating = false;

// -------- ī�޶� --------
glm::vec3 cameraOffset(0.0f, 0.3f, 0.5f); // ���� ī�޶� ������ ���� �Ÿ� (��, ��)
glm::vec3 cameraPos = spherePosition + cameraOffset; // �ʱ� ī�޶� ��ġ
glm::vec3 initialCameraDir = glm::normalize(-cameraOffset); // ���� �ٶ󺸴� �ʱ� ����
float cameraAngle = 90.0f; // ī�޶��� ȸ�� ���� (�ʱⰪ 0��)

// -------- ��� --------
glm::mat4 GoalTransForm;

float GoalLocationX = 0.0f;
float GoalLocationY = 0.0f;
float GoalLocationZ = 0.0f;

bool Stage3State = false; // ��� ������ ����

// -------- ��ֹ� --------
glm::vec3 obstacle1Position(2.32f, 0.55f, -10.0f); // ��ֹ� 1�� �ʱ� ��ġ
glm::vec3 obstacle2Position(0.85f, 0.55f, -3.0f); // ��ֹ� 2�� �ʱ� ��ġ
float obstacleMoveSpeed = 0.02f; // ��ֹ� �̵� �ӵ�
float obstacleMoveRange = 1.0f; // ��ֹ� �̵� ����
bool obstacle1MovingRight = true; // ��ֹ� 1 �̵� ����
bool obstacle2MovingRight = false; // ��ֹ� 2 �̵� ����


// -------- �浹 --------
bool isCollisionDetected1 = false; // �浹 ���� ���� ����
bool isCollisionDetected2 = false;
bool isCollisionDetected3 = false;
bool isCollisionDetected4 = false;

// ���������� ��谪
float stage1BoundaryMinX = -0.95f, stage1BoundaryMaxX = 0.95f;
float stage1BoundaryMinZ = -10.45f, stage1BoundaryMaxZ = 0.45f;

float stage2BoundaryMinX = 1.05f, stage2BoundaryMaxX = 2.95f;
float stage2BoundaryMinZ = -15.45f, stage2BoundaryMaxZ = -4.55f;

float stage3BoundaryMinZ = -20.45f;

float boundaryY = 0.55f;  // Y�� ������ (����ü ���)

// AABB ���� (��ü�� ���δ� AABB)
struct AABB {
	glm::vec3 min;  // �ּ���
	glm::vec3 max;  // �ִ���
};
// �浹 �ڽ� ����
glm::vec3 goalBoxMin, goalBoxMax;
// ����� �浹�ڽ��� ����ϴ� �Լ�
bool checkAABBCollision(const AABB& a, const AABB& b);
AABB createGolfBallAABB(const glm::vec3& center, float radius);
void checkCollision();

// -------- �� --------
int currentMapStage = 0; // ���� �� ��������
int saveMapStage = 1;

int state = 0;

// -------- �۾� --------
void renderBitmapString(float x, float y, float z, float scale, const char* string);

// -------- �ð� --------
int sec = 0;
int min = 0;
int hour = 0;

bool TimeOn = false;

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
	glutTimerFunc(1000, TimerUpdate, 2);
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

void InitBuffer()
{
	glGenVertexArrays(1, &VAO); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glGenBuffers(3, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�


	glBindVertexArray(VAO);

	// 1. ���� ��ġ ������ ó��
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 2. ���� ������ ó��
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColor), vertexColor, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// 3. �ؽ�ó ��ǥ ������ ó��
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); // location 2�� ����
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // VAO ����ε�

	// �ؽ�ó �ε�
	textureID[0] = LoadTexture("title.bmp");
	textureID[1] = LoadTexture("ending.bmp");
}

GLuint LoadTexture(const char* filename)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true); //--- �̹������Ųٷ������ٸ��߰�
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image == nullptr)
	{
		std::cerr << "ERROR: �ؽ�ó �ε� ����: " << filename << std::endl;
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return texture;
}

GLvoid drawScene() {

	glUseProgram(shaderID);
	glBindVertexArray(VAO);

	glClearColor(0.698f, 0.886f, 0.871f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	hFlag ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);//��������
	yFlag ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//���̾� ��ü

	unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//���� ��ȯ ��İ��� ���̴��� uniform mat4 modelTransform���� �Ѱ���
	unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//���� ����
	unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//���� ����
	unsigned int isSphereLocation = glGetUniformLocation(shaderID, "isSphere");
	unsigned int sphereColorLocation = glGetUniformLocation(shaderID, "sphereColor");
	unsigned int vColorLocation = glGetUniformLocation(shaderID, "vColor");
	unsigned int textureOnLocation = glGetUniformLocation(shaderID, "TextureOn");
	unsigned int ourTextureLocation = glGetUniformLocation(shaderID, "ourTexture");

	//���� ����
	glm::mat4 kTransform = glm::mat4(1.0f);
	kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -5.0f));
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

	// ī�޶� ��ġ ���
	float radius = glm::length(cameraOffset); // ���� ī�޶� ���� �Ÿ� (������)
	float cameraX = spherePosition.x + radius * cos(glm::radians(cameraAngle)); // ȸ���� X ��ǥ
	float cameraZ = spherePosition.z + radius * sin(glm::radians(cameraAngle)); // ȸ���� Z ��ǥ
	float cameraY = spherePosition.y + cameraOffset.y; // Y���� ����
	cameraPos = glm::vec3(cameraX, cameraY, cameraZ); // ���ŵ� ī�޶� ��ġqq

	// ���� ��ȯ
	glm::mat4 vTransform = glm::mat4(1.0f); // ī�޶� ���� �ٶ󺸵��� ����
	glm::vec3 cameraDirection = spherePosition; // ���� ���� ��ġ�� �ٶ�
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // ī�޶��� ���� ����
	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

	// �� ����
	if (currentMapStage == 0) {
		TimeOn = false;

		// �ؽ�ó�� ����ϵ��� ����
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID[0]);
		glUniform1i(ourTextureLocation, 0);
		glUniform1i(textureOnLocation, 1); // TextureOn�� Ȱ��ȭ

		// ī�޶�
		float cameraZ = 1.0f; // ī�޶� Z�� ���� ���⿡ ��ġ (��: Z = 10)
		cameraPos = glm::vec3(0.0f, 0.0f, cameraZ); // Y, X�� 0���� �ΰ�, Z�� ����

		// ���� ��ȯ (������ �ٶ󺸴� ī�޶�)
		glm::mat4 vTransform = glm::mat4(1.0f);
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); // ������ �ٶ�
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // ī�޶��� ���� ���� (Y�� ����)
		vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp); // ������ �����ϰ� �ٶ󺸴� ī�޶�
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

		// Ÿ��Ʋ(���)
		glm::mat4 shapeTransForm = glm::mat4(1.0f);  // ��ȯ ��� ����
		shapeTransForm = glm::scale(shapeTransForm, glm::vec3(3.0f, 5.0f, 0.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm));

		// ���� ������ü �׸���
		glDrawArrays(GL_QUADS, 0, 24);

	}
	else if (currentMapStage == 1) {
		TimeOn = true;
		
		glm::mat4 shapeTransForm = glm::mat4(1.0f);//��ȯ ��� ���� T
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm));//��ȯ ����� ���̴��� ����
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����		
		glUniform1i(textureOnLocation, 0); // TextureOn�� ��Ȱ��ȭ
		glDrawArrays(GL_QUADS, 0, 24); //������ü

		// ��� ����
		GoalLocationX = 0.0f;
		GoalLocationY = 0.5f;
		GoalLocationZ = -10.0f;

		GoalTransForm = glm::mat4(1.0f);
		GoalTransForm = glm::translate(GoalTransForm, glm::vec3(GoalLocationX, GoalLocationY, GoalLocationZ));
		GoalTransForm = glm::scale(GoalTransForm, glm::vec3(0.05f, 2.0f, 0.01f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(GoalTransForm));
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
		glDrawArrays(GL_QUADS, 0, 24);
	}
	else if (currentMapStage == 2 || currentMapStage == 3) {
		TimeOn = true;

		// ù ��° ������ü
		glm::mat4 shapeTransForm = glm::mat4(1.0f); // �⺻ ��ȯ ��� ����
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm)); // ��ȯ ����� ���̴��� ����
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
		glUniform1i(textureOnLocation, 0); // TextureOn�� ��Ȱ��ȭ
		glDrawArrays(GL_QUADS, 0, 24); // ������ü

		// �� ��° ������ü (�����ʿ� ��ġ)
		glm::mat4 shapeTransForm2 = glm::mat4(1.0f); // �⺻ ��ȯ ��� ����
		shapeTransForm2 = glm::translate(shapeTransForm2, glm::vec3(2.0f, 0.0f, -5.0f)); // ���������� �̵�
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm2)); // ��ȯ ����� ���̴��� ����
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
		glDrawArrays(GL_QUADS, 0, 24); // ������ü

		// ��� ����
		if ((currentMapStage == 3 && !Stage3State) || (currentMapStage == 2)) {
			GoalLocationX = 2.0f;
			GoalLocationY = 0.5f;
			GoalLocationZ = -15.0f;
		}

		GoalTransForm = glm::mat4(1.0f);
		GoalTransForm = glm::translate(GoalTransForm, glm::vec3(GoalLocationX, GoalLocationY, GoalLocationZ));
		GoalTransForm = glm::scale(GoalTransForm, glm::vec3(0.05f, 2.0f, 0.01f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(GoalTransForm));
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
		glUniform1i(textureOnLocation, 0); // TextureOn�� ��Ȱ��ȭ
		glDrawArrays(GL_QUADS, 0, 24);

		// ��ֹ� 1 ������
		glm::mat4 obstacle1Transform = glm::mat4(1.0f);
		obstacle1Transform = glm::translate(obstacle1Transform, obstacle1Position);
		obstacle1Transform = glm::scale(obstacle1Transform, glm::vec3(0.2f, 0.5f, 0.1f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(obstacle1Transform));
		glUniform1i(isSphereLocation, 0); // ������ü�� ����
		glDrawArrays(GL_QUADS, 0, 24);

		// ��ֹ� 2 ������
		glm::mat4 obstacle2Transform = glm::mat4(1.0f);
		obstacle2Transform = glm::translate(obstacle2Transform, obstacle2Position);
		obstacle2Transform = glm::scale(obstacle2Transform, glm::vec3(0.3f, 0.5f, 0.07f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(obstacle2Transform));
		glUniform1i(isSphereLocation, 0); // ������ü�� ����
		glDrawArrays(GL_QUADS, 0, 24);

		if (currentMapStage == 3) {
			if (Stage3State || spherePosition.z <= -13.0f) {
				GoalLocationX = 0.0f;
				GoalLocationY = 0.5f;
				GoalLocationZ = 0.0f;

				Stage3State = true; // ��� �̵��ߴٴ� ����
			}
		}
	}
	else if (currentMapStage == 4) {
		TimeOn = true;

		// ù ��° �簢��
		glm::mat4 shapeTransForm = glm::mat4(1.0f);//��ȯ ��� ���� T
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm));//��ȯ ����� ���̴��� ����
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
		glUniform1i(textureOnLocation, 0); // TextureOn�� ��Ȱ��ȭ
		glDrawArrays(GL_QUADS, 0, 24); //������ü

		// �� ��° �簢��(���� �Ʒ�)
		glm::mat4 shapeTransForm2 = glm::mat4(1.0f); // �⺻ ��ȯ ��� ����
		shapeTransForm2 = glm::translate(shapeTransForm2, glm::vec3(0.0f, -2.0f, -10.0f)); // �Ʒ������� �̵�
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm2)); // ��ȯ ����� ���̴��� ����
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
		glUniform1i(textureOnLocation, 0); // TextureOn�� ��Ȱ��ȭ
		glDrawArrays(GL_QUADS, 0, 24); // ������ü

		// ��� ����
		GoalLocationX = 0.0f;
		GoalLocationY = -1.5f;
		GoalLocationZ = -20.0f;

		GoalTransForm = glm::mat4(1.0f);
		GoalTransForm = glm::translate(GoalTransForm, glm::vec3(GoalLocationX, GoalLocationY, GoalLocationZ));
		GoalTransForm = glm::scale(GoalTransForm, glm::vec3(0.05f, 2.0f, 0.01f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(GoalTransForm));
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
		glUniform1i(textureOnLocation, 0); // TextureOn�� ��Ȱ��ȭ
		glDrawArrays(GL_QUADS, 0, 24);

		// ��¥ ��� ����
		glm::mat4 FakeGoalTransForm = glm::mat4(1.0f);
		FakeGoalTransForm = glm::translate(FakeGoalTransForm, glm::vec3(0.0f, 0.5f, -10.0f));
		FakeGoalTransForm = glm::scale(FakeGoalTransForm, glm::vec3(0.05f, 2.0f, 0.01f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(FakeGoalTransForm));
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
		glUniform1i(textureOnLocation, 0); // TextureOn�� ��Ȱ��ȭ
		glDrawArrays(GL_QUADS, 0, 24);

	}
	else if (currentMapStage == 5) {
		TimeOn = false;

		// �ؽ�ó�� ����ϵ��� ����
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID[1]);
		glUniform1i(ourTextureLocation, 0);
		glUniform1i(textureOnLocation, 1); // TextureOn�� Ȱ��ȭ

		// ī�޶�
		float cameraZ = 1.0f; // ī�޶� Z�� ���� ���⿡ ��ġ (��: Z = 10)
		cameraPos = glm::vec3(0.0f, 0.0f, cameraZ); // Y, X�� 0���� �ΰ�, Z�� ����

		// ���� ��ȯ (������ �ٶ󺸴� ī�޶�)
		glm::mat4 vTransform = glm::mat4(1.0f);
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); // ������ �ٶ�
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // ī�޶��� ���� ���� (Y�� ����)
		vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp); // ������ �����ϰ� �ٶ󺸴� ī�޶�
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

		// ����(���)
		glm::mat4 shapeTransForm = glm::mat4(1.0f);  // ��ȯ ��� ����
		shapeTransForm = glm::scale(shapeTransForm, glm::vec3(3.0f, 5.0f, 0.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm));

		glDrawArrays(GL_QUADS, 0, 24);
	}

	// ���� ���� ��ȯ ���
	glm::mat4 sphereModel = glm::mat4(1.0f);
	sphereModel = glm::translate(sphereModel, spherePosition); // ���� ��ġ�� ������ ����
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(sphereModel));
	glUniform1i(isSphereLocation, 1); // ���� ��, isSphere�� 1�� ����
	glUniform3f(sphereColorLocation, 1.0f, 1.0f, 1.0f); // �� ����

	if (currentMapStage != 0 && currentMapStage != 5) {
		// GLU �� ���� �� �׸���
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL); // ��Ÿ��(���̾�LINE, ��FILL)
		gluQuadricNormals(qobj, GLU_SMOOTH); // �ε巯�� ���
		gluQuadricOrientation(qobj, GLU_OUTSIDE); // �ܺ� ���� ����(�̷��� ī�޶� �� �ۿ��� �� ǥ���� ���� ��)
		gluSphere(qobj, 0.05, 50, 50); // ������ 0.05, 50���� ���׸�Ʈ�� ����

		renderBitmapString(0.5f, 0.8f, 0.05f, 0.005f, "key");
		renderBitmapString(0.5f, 0.7f, 0.05f, 0.005f, "current");

		// �簢�� 1: move_len
		float rectHeight1 = move_len * 0.5f; // �̵� �Ÿ��� ���
		drawRectangle(0.95f, 1.0f, 0.05f, rectHeight1, 1.0f, 0.0f, 0.0f); // ������
		// �ؽ�Ʈ ��� (ȭ�� ��ġ, �۲�, �ؽ�Ʈ ����)
		renderBitmapString(0.9f, 0.9f, 0.05f, 0.005f, "distance");
		renderBitmapString(0.9f, 0.8f, 0.05f, 0.005f, "1  ~  4");
		std::ostringstream oss1;
		oss1 << move_len;  // move_len ���� �ؽ�Ʈ�� ��ȯ
		renderBitmapString(0.95f, 0.7f, 0.05f, 0.005f, oss1.str().c_str());

		// �簢�� 2: moveSpeed
		float rectHeight2 = moveSpeed * 10.0f; // �ӵ��� ���
		drawRectangle(1.35f, 1.0f, 0.05f, rectHeight2, 0.0f, 0.0f, 1.0f); // �Ķ���
		// �ؽ�Ʈ ��� (ȭ�� ��ġ, �۲�, �ؽ�Ʈ ����)
		renderBitmapString(1.3f, 0.9f, 0.05f, 0.005f, "speed");
		renderBitmapString(1.3f, 0.8f, 0.05f, 0.005f, "up, down");
		std::ostringstream oss2;
		oss2 << moveSpeed;  // moveSpeed ���� �ؽ�Ʈ�� ��ȯ
		renderBitmapString(1.35f, 0.7f, 0.05f, 0.005f, oss2.str().c_str());
	}
	else if (currentMapStage == 0) {
		glUniform1i(textureOnLocation, 0); // TextureOn�� ��Ȱ��ȭ
		renderBitmapString(-2.0f, -0.4f, 0.5f, 0.005f, "* Game Controls *");
		renderBitmapString(-2.0f, -0.8f, 0.5f, 0.005f, "WASD : Ball Movement");
		renderBitmapString(-2.0f, -1.0f, 0.5f, 0.005f, "R : Reset Current Stage");
		renderBitmapString(-2.0f, -1.2f, 0.5f, 0.005f, "C : Reset Camera View");
		renderBitmapString(-2.0f, -1.4f, 0.5f, 0.005f, "Q : Return to Title and Quit Game");
		renderBitmapString(-2.0f, -1.6f, 0.5f, 0.005f, "1234 : Ball Movement Distance");
		renderBitmapString(-2.0f, -1.8f, 0.5f, 0.005f, "up down : Ball Speed");
	}
	else if (currentMapStage == 5) {
		glUniform1i(textureOnLocation, 0); // TextureOn�� ��Ȱ��ȭ
		// ��� �ð� �ؽ�Ʈ ���
		renderBitmapString(-2.0f, 1.3f, 0.5f, 0.005f, "Time: ");
		std::ostringstream timeStream1;
		timeStream1 << hour;
		renderBitmapString(-1.5f, 1.3f, 0.5f, 0.005f, timeStream1.str().c_str());
		renderBitmapString(-1.3f, 1.3f, 0.5f, 0.005f, " : ");
		std::ostringstream timeStream2;
		timeStream2 << min;
		renderBitmapString(-1.1f, 1.3f, 0.5f, 0.005f, timeStream2.str().c_str());
		renderBitmapString(-0.9f, 1.3f, 0.5f, 0.005f, " : ");
		std::ostringstream timeStream3;
		timeStream3 << sec;
		renderBitmapString(-0.7f, 1.3f, 0.5f, 0.005f, timeStream3.str().c_str());
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid KeyBoard(unsigned char key, int x, int y) {
	if (isAnimating) return; // �ִϸ��̼� ���̸� �Է� ����

	switch (key) {
	case 'h':
		hFlag = 1 - hFlag; // ��� ���
		break;
	case 'y':
		yFlag = !yFlag; // ��� ���
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
	case 13: // ���� Ű
		if (currentMapStage == 0) currentMapStage = saveMapStage;
		else if (currentMapStage == 5) {
			currentMapStage = 0; // �������� Ÿ��Ʋ�� �̵�
			resetBallPosition();
		}
		break;
	case 'W':
	case 'w':
		if (currentMapStage != 0 && currentMapStage != 5) {
			targetPosition.z -= move_len; // ��ǥ ��ġ ����
		}
		break;
	case 'A':
	case 'a':
		if (currentMapStage != 0 && currentMapStage != 5) {
			targetPosition.x -= move_len; // ��ǥ ��ġ ����
		}
		break;
	case 'S':
	case 's':
		if (currentMapStage != 0 && currentMapStage != 5) {
			targetPosition.z += move_len; // ��ǥ ��ġ ����
		}
		break;
	case 'D':
	case 'd':
		if (currentMapStage != 0 && currentMapStage != 5) {
			targetPosition.x += move_len; // ��ǥ ��ġ ����
		}
		break;
	case 'R':
	case 'r':
		resetBallPosition();

		if (currentMapStage == 3) {
			Stage3State = false;
		}
		break;
	case 'C':
	case 'c':
		cameraAngle = 90.0f;
		break;
	case 'Q':
	case 'q':
		if (currentMapStage == 0) {
			exit(-1);
		}
		else {
			saveMapStage = currentMapStage;
			currentMapStage = 0;
		}
	}

	// ���������� ��� ����
	restrictTargetPosition();

	// ��ǥ ��ġ�� ���� ��ġ�� �����ϸ� �ִϸ��̼� ����
	if (glm::distance(spherePosition, targetPosition) < 0.01f) {
		isAnimating = false;
	}
	else {
		isAnimating = true; // �ִϸ��̼� ����
	}

	glutPostRedisplay();
}

GLvoid SpecialKeyBoard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		cameraAngle = cameraAngle - 1.f;
		if (cameraAngle > 360)cameraAngle = 0.f;
		break;
	case GLUT_KEY_RIGHT:
		cameraAngle = cameraAngle + 1.f;
		if (cameraAngle > 360)cameraAngle = 0.f;
		break;
	case GLUT_KEY_UP:
		if (moveSpeed + 0.05f <= 0.20f) {
			moveSpeed += 0.05f;
		}
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
	// ���� ��ǥ ��ġ�� �������� �ʾ����� ���� ó��
	if (glm::distance(spherePosition, targetPosition) > 0.01f) {
		glm::vec3 direction = glm::normalize(targetPosition - spherePosition);
		spherePosition += direction * moveSpeed;

		// ��� ���� ����
		targetPosition.y = boundaryY; // Y�� ����

		// ��ǥ ��ġ�� �Ѿ�� �ʵ��� Ŭ����
		if (glm::distance(spherePosition, targetPosition) < moveSpeed) {
			spherePosition = targetPosition;
			isAnimating = false; // �ִϸ��̼� ����, �Է� ��� ����
		}
	}

	// ī�޶� ���� ���� �̵�
	cameraPos = spherePosition + cameraOffset;


	if (currentMapStage == 2 || currentMapStage == 3) {
		// ��ֹ� ��ġ ������Ʈ
		updateObstaclePosition();

		// ��ֹ��� �浹 �˻�
		checkObstacleCollision();
	}

	// �浹 �˻� �Լ� ȣ��
	checkCollision();

	glutTimerFunc(10, TimerFunc, 1); // Ÿ�̸� ����



	glutPostRedisplay(); // ȭ�� ����
}

GLvoid TimerUpdate(int x) {
	if(TimeOn){
		sec++; // 1�� ����
		if (sec >= 60) {
			sec = 0;
			min++;
		}
		if (min >= 60) {
			min = 0;
			hour++;
		}

		// Ÿ�̸� �ٽ� ���� (1�� �� �ٽ� ȣ��)
		glutTimerFunc(1000, TimerUpdate, 2);
	}
}

// ��� AABB
void updateGoalBoundingBox(glm::mat4 GoalTransForm) {
	// ������ ������ �����Ͽ� �浹 �ڽ� ũ�� ���
	glm::vec3 scale = glm::vec3(GoalTransForm[0][0], GoalTransForm[1][1], GoalTransForm[2][2]);

	// �̵� ������ �����Ͽ� �浹 �ڽ� ��ġ ���
	glm::vec3 position = glm::vec3(GoalTransForm[3][0], GoalTransForm[3][1], GoalTransForm[3][2]);

	// �浹 �ڽ� �ּ�, �ִ� �� ���
	goalBoxMin = position - scale / 2.0f;  // ũ���� ������ ���� �ּ���
	goalBoxMax = position + scale / 2.0f;  // ũ���� ������ ���ϸ� �ִ���
}

// �� AABB �� �浹�� �˻��ϴ� �Լ�
bool checkAABBCollision(const AABB& a, const AABB& b) {
	if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
	if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
	if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
	return true;
}

// �������� AABB ����
AABB createGolfBallAABB(const glm::vec3& center, float radius) {
	AABB aabb;
	aabb.min = center - glm::vec3(radius, radius, radius);
	aabb.max = center + glm::vec3(radius, radius, radius);
	return aabb;
}

void checkCollision() {
	if (currentMapStage == 1 && isCollisionDetected1) {
		return; // �浹�� �̹� �߻������� �߰����� �浹 ó�� �� ��
	}
	else if (currentMapStage == 2 && isCollisionDetected2) {
		return; // �浹�� �̹� �߻������� �߰����� �浹 ó�� �� ��
	}
	else if (currentMapStage == 3 && isCollisionDetected3) {
		return; // �浹�� �̹� �߻������� �߰����� �浹 ó�� �� ��
	}
	else if (currentMapStage == 4 && isCollisionDetected4) {
		return; // �浹�� �̹� �߻������� �߰����� �浹 ó�� �� ��
	}

	// ������ AABB ���� (���� ��ġ�� ���)
	float golfBallRadius = 0.05f; // �������� ������
	AABB golfBallAABB = createGolfBallAABB(spherePosition, golfBallRadius);

	// ��� AABB ����
	updateGoalBoundingBox(GoalTransForm); // GoalTransform�� ������� goalBoxMin, goalBoxMax ������Ʈ
	AABB goalAABB = { goalBoxMin, goalBoxMax };

	// �浹 �˻�
	if (checkAABBCollision(golfBallAABB, goalAABB)) {
		std::cout << "�浹 �߻�!" << std::endl;
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
	}
}

// ��ǥ ��ġ ��� ���� �Լ�
void restrictTargetPosition() {
	if (currentMapStage == 1) {
		boundaryY = 0.55f;
		if (targetPosition.x < stage1BoundaryMinX) targetPosition.x = stage1BoundaryMinX;
		if (targetPosition.x > stage1BoundaryMaxX) targetPosition.x = stage1BoundaryMaxX;
		if (targetPosition.z < stage1BoundaryMinZ) targetPosition.z = stage1BoundaryMinZ;
		if (targetPosition.z > stage1BoundaryMaxZ) targetPosition.z = stage1BoundaryMaxZ;
	}
	else if (currentMapStage == 2 || currentMapStage == 3) {
		boundaryY = 0.55f;
		if (targetPosition.z >= -4.5f) {
			if (targetPosition.x < stage1BoundaryMinX) targetPosition.x = stage1BoundaryMinX;
			if (targetPosition.x > stage1BoundaryMaxX) targetPosition.x = stage1BoundaryMaxX;
			if (targetPosition.z > stage1BoundaryMaxZ) targetPosition.z = stage1BoundaryMaxZ;
		}
		else if (targetPosition.x < 1.0f && targetPosition.z >= -10.5f) {
			if (targetPosition.x < 2.0f && targetPosition.z >= -15.0f) {
				if (targetPosition.x < stage1BoundaryMinX) targetPosition.x = stage1BoundaryMinX;
				if (targetPosition.z < stage1BoundaryMinZ) targetPosition.z = stage1BoundaryMinZ;
			}
		}
		else if (targetPosition.x >= 1.0f && targetPosition.z >= -10.5f) {
			if (targetPosition.z >= -15.0f) {
				if (targetPosition.x > stage2BoundaryMaxX) targetPosition.x = stage2BoundaryMaxX;
				if (targetPosition.z > stage2BoundaryMaxZ) targetPosition.z = stage2BoundaryMaxZ;
			}
		}
		else if (targetPosition.z >= -20.0f) {
			if (targetPosition.x < stage2BoundaryMinX) targetPosition.x = stage2BoundaryMinX;
			if (targetPosition.x > stage2BoundaryMaxX) targetPosition.x = stage2BoundaryMaxX;
			if (targetPosition.z < stage2BoundaryMinZ) targetPosition.z = stage2BoundaryMinZ;
		}
	}
	else if (currentMapStage == 4) {
		if (targetPosition.z < stage1BoundaryMinZ) boundaryY = -1.45f;
		else boundaryY = 0.55f;
		targetPosition.x = glm::clamp(targetPosition.x, stage1BoundaryMinX, stage1BoundaryMaxX);
		targetPosition.z = glm::clamp(targetPosition.z, stage3BoundaryMinZ, stage1BoundaryMaxX);
	}
}

void resetBallPosition() {
	spherePosition = glm::vec3(0.0f, 0.55f, 0.0f);
	targetPosition = spherePosition;
	cameraPos = spherePosition + cameraOffset;
	cameraAngle = 90.0f;
	isAnimating = false;
	isCollisionDetected1 = false;
	isCollisionDetected2 = false;
	isCollisionDetected3 = false;
	isCollisionDetected4 = false;
	moveSpeed = 0.02f;
	move_len = 1.0f;
	sec = 0, min = 0, sec = 0;
}

// ��ֹ� ��ġ ������Ʈ
void updateObstaclePosition() {
	// ��ֹ� 1 �¿� �̵�
	if (obstacle1MovingRight) {
		obstacle1Position.x += obstacleMoveSpeed;
		if (obstacle1Position.x > 2.0f + obstacleMoveRange) obstacle1MovingRight = false;
	}
	else {
		obstacle1Position.x -= obstacleMoveSpeed;
		if (obstacle1Position.x < 2.0f - obstacleMoveRange) obstacle1MovingRight = true;
	}

	// ��ֹ� 2 �¿� �̵�
	if (obstacle2MovingRight) {
		obstacle2Position.x += obstacleMoveSpeed;
		if (obstacle2Position.x > 0.0f + obstacleMoveRange) obstacle2MovingRight = false;
	}
	else {
		obstacle2Position.x -= obstacleMoveSpeed;
		if (obstacle2Position.x < 0.0f - obstacleMoveRange) obstacle2MovingRight = true;
	}
}

// ��ֹ��� �浹 �˻�
void checkObstacleCollision() {
	// ��ֹ� AABB ����
	AABB obstacle1AABB = createGolfBallAABB(obstacle1Position, 0.2f);
	AABB obstacle2AABB = createGolfBallAABB(obstacle2Position, 0.2f);

	// ���� AABB ����
	float golfBallRadius = 0.05f;
	AABB golfBallAABB = createGolfBallAABB(spherePosition, golfBallRadius);

	// ��ֹ� 1�� �浹 �˻�
	if (checkAABBCollision(golfBallAABB, obstacle1AABB)) {
		spherePosition.z += move_len - 0.2f; // ���� �ڷ� �б�
		targetPosition = spherePosition; // ��ǥ ��ġ ����
		isAnimating = false;
	}

	// ��ֹ� 2�� �浹 �˻�
	if (checkAABBCollision(golfBallAABB, obstacle2AABB)) {
		spherePosition.z += move_len - 0.2f; // ���� �ڷ� �б�
		targetPosition = spherePosition; // ��ǥ ��ġ ����
		isAnimating = false;
	}
}

// �簢���� �׸��� �Լ�
void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // ȭ�� ��ǥ�� ����

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(r, g, b); // ���� ����
	glBegin(GL_QUADS);
	glVertex2f(x, y);                     // ���ϴ�
	glVertex2f(x + width, y);            // ���ϴ�
	glVertex2f(x + width, y + height / 2);   // ����
	glVertex2f(x, y + height / 2);           // �»��
	glEnd();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// �ؽ�Ʈ�� �׸��� �Լ�
void renderBitmapString(float x, float y, float z, float scale, const char* string) {
	glPushMatrix();
	glRasterPos3f(x, y, z);  // �ؽ�Ʈ ��ġ ����
	glScalef(scale, scale, scale);  // �ؽ�Ʈ ũ�� ����
	for (const char* c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);  // �� ���� ���
	}
	glPopMatrix();
}