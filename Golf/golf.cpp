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

void InitBuffer();
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void checkCollision();
void restrictTargetPosition();

GLchar* vertexSource, * fragmentSource;
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO, VBO[2];

bool hFlag = false;
bool yFlag = true;

float yAngle{};

// -------- �� --------
glm::vec3 spherePosition(0.0f, 0.55f, 0.0f); // ���� �ʱ� ��ġ�� �����ϴ� ����

glm::vec3 targetPosition = spherePosition; // ��ǥ ��ġ
float moveSpeed = 0.02f; // �̵� �ӵ�

// �ִϸ��̼� ���� ���¸� ��Ÿ���� �÷���
bool isAnimating = false;

// -------- ī�޶� --------
glm::vec3 cameraOffset(0.0f, 0.3f, 0.5f); // ���� ī�޶� ������ ���� �Ÿ� (��, ��)
glm::vec3 cameraPos = spherePosition + cameraOffset; // �ʱ� ī�޶� ��ġ
glm::vec3 initialCameraDir = glm::normalize(-cameraOffset); // ���� �ٶ󺸴� �ʱ� ����

// -------- ��� --------
glm::mat4 GoalTransForm;

float GoalLocationX = 0.0f;
float GoalLocationY = 0.0f;
float GoalLocationZ = 0.0f;

bool Stage3State = false; // ��� ������ ����

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
int currentMapStage = 2; // ���� �� ��������

// �̵� �Ÿ�
float move_len = 1.0f;

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


	//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 0���� ��밡���ϰ� ��
	glEnableVertexAttribArray(0);
	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	//--- colors �迭�� ������: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexColor), vertexColor, GL_STATIC_DRAW);
	//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 1���� ��� �����ϰ� ��.
	glEnableVertexAttribArray(1);

}

GLvoid drawScene() {

	glUseProgram(shaderID);
	glBindVertexArray(VAO);

	glClearColor(0.698f, 0.886f, 0.871f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	hFlag ? glDisable(GL_DEPTH_TEST) : glEnable(GL_DEPTH_TEST);//��������
	yFlag ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//���̾� ��ü

	unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//���� ��ȯ ��İ��� ���̴��� uniform mat4 modelTransform���� �Ѱ���
	unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//���� ����
	unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//���� ����
	unsigned int isSphereLocation = glGetUniformLocation(shaderID, "isSphere");
	unsigned int sphereColorLocation = glGetUniformLocation(shaderID, "sphereColor");

	//���� ����
	glm::mat4 kTransform = glm::mat4(1.0f);
	kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -5.0f));
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

	// ���� ��ȯ
	glm::mat4 vTransform = glm::mat4(1.0f);
	// ī�޶� ���� �ٶ󺸵��� ����
	glm::vec3 cameraDirection = spherePosition; // ���� ���� ��ġ�� �ٶ�
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // ī�޶��� ���� ����

	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

	// �� ����
	if (currentMapStage == 1) {
		glm::mat4 shapeTransForm = glm::mat4(1.0f);//��ȯ ��� ���� T
		shapeTransForm = glm::rotate(shapeTransForm, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0));//y�� ȸ��
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm));//��ȯ ����� ���̴��� ����
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
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
		// ù ��° ������ü
		glm::mat4 shapeTransForm = glm::mat4(1.0f); // �⺻ ��ȯ ��� ����
		shapeTransForm = glm::rotate(shapeTransForm, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0)); // y�� ȸ��
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm)); // ��ȯ ����� ���̴��� ����
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
		glDrawArrays(GL_QUADS, 0, 24); // ������ü

		// �� ��° ������ü (�����ʿ� ��ġ)
		glm::mat4 shapeTransForm2 = glm::mat4(1.0f); // �⺻ ��ȯ ��� ����
		shapeTransForm2 = glm::rotate(shapeTransForm2, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0)); // y�� ȸ��
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
		glDrawArrays(GL_QUADS, 0, 24);

		if (currentMapStage == 3) {
			if (Stage3State || spherePosition.x <= -8.5f) {
				GoalLocationX = 0.0f;
				GoalLocationY = 0.5f;
				GoalLocationZ = 0.0f;

				Stage3State = true; // ��� �̵��ߴٴ� ����
			}
		}
	}
	else if (currentMapStage == 4) {
		// ù ��° �簢��
		glm::mat4 shapeTransForm = glm::mat4(1.0f);//��ȯ ��� ���� T
		shapeTransForm = glm::rotate(shapeTransForm, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0));//y�� ȸ��
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm));//��ȯ ����� ���̴��� ����
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
		glDrawArrays(GL_QUADS, 0, 24); //������ü
	
		// �� ��° �簢��(���� �Ʒ�)
		glm::mat4 shapeTransForm2 = glm::mat4(1.0f); // �⺻ ��ȯ ��� ����
		shapeTransForm2 = glm::rotate(shapeTransForm2, glm::radians(yAngle), glm::vec3(0.0, 1.0, 0.0)); // y�� ȸ��
		shapeTransForm2 = glm::translate(shapeTransForm2, glm::vec3(0.0f, -2.0f, -10.0f)); // �Ʒ������� �̵�
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm2)); // ��ȯ ����� ���̴��� ����
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
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
		glDrawArrays(GL_QUADS, 0, 24);

		// ��¥ ��� ����
		glm::mat4 FakeGoalTransForm = glm::mat4(1.0f);
		FakeGoalTransForm = glm::translate(FakeGoalTransForm, glm::vec3(0.0f, 0.5f, -10.0f));
		FakeGoalTransForm = glm::scale(FakeGoalTransForm, glm::vec3(0.05f, 2.0f, 0.01f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(FakeGoalTransForm));
		glUniform1i(isSphereLocation, 0); // ������ü�� �� isSphere�� 0���� ����
		glDrawArrays(GL_QUADS, 0, 24);
	
	}


	// ���� ���� ��ȯ ���
	glm::mat4 sphereModel = glm::mat4(1.0f);
	sphereModel = glm::translate(sphereModel, spherePosition); // ���� ��ġ�� ������ ����
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(sphereModel));
	glUniform1i(isSphereLocation, 1); // ���� ��, isSphere�� 1�� ����
	glUniform3f(sphereColorLocation, 1.0f, 1.0f, 1.0f); // �� ����


	// GLU �� ���� �� �׸���
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL); // ��Ÿ��(���̾�LINE, ��FILL)
	gluQuadricNormals(qobj, GLU_SMOOTH); // �ε巯�� ���
	gluQuadricOrientation(qobj, GLU_OUTSIDE); // �ܺ� ���� ����(�̷��� ī�޶� �� �ۿ��� �� ǥ���� ���� ��)
	gluSphere(qobj, 0.05, 50, 50); // ������ 0.05, 50���� ���׸�Ʈ�� ����
	

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

int state = 0;
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
	case 'W':
	case 'w':
		targetPosition.z -= move_len; // ��ǥ ��ġ ����
		break;
	case 'A':
	case 'a':
		targetPosition.x -= move_len; // ��ǥ ��ġ ����
		break;
	case 'S':
	case 's':
		targetPosition.z += move_len; // ��ǥ ��ġ ����
		break;
	case 'D':
	case 'd':
		targetPosition.x += move_len; // ��ǥ ��ġ ����
		break;
	case 'R':
	case 'r':
		isAnimating = false;

		spherePosition.x = 0.0f;
		spherePosition.y = 0.55f;
		spherePosition.z = 0.0f;

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

	// ���������� ��� ����
	restrictTargetPosition();

	targetPosition.y = boundaryY; // Y�� ����

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
	// ���� ��ǥ ��ġ�� �������� �ʾ����� ���� ó��
	if (glm::distance(spherePosition, targetPosition) > 0.01f) {
		glm::vec3 direction = glm::normalize(targetPosition - spherePosition);
		spherePosition += direction * moveSpeed;

		// ��� ���� ����
		restrictTargetPosition();
		spherePosition.y = boundaryY; // Y�� ����

		// ��ǥ ��ġ�� �Ѿ�� �ʵ��� Ŭ����
		if (glm::distance(spherePosition, targetPosition) < moveSpeed) {
			spherePosition = targetPosition;
			isAnimating = false; // �ִϸ��̼� ����, �Է� ��� ����
		}
	}

	// ī�޶� ���� ���� �̵�
	cameraPos = spherePosition + cameraOffset;

	// �浹 �˻� �Լ� ȣ��
	checkCollision();

	glutTimerFunc(10, TimerFunc, 1); // Ÿ�̸� ����
	glutPostRedisplay(); // ȭ�� ����
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
	else if(currentMapStage == 2 && isCollisionDetected2) {
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
		}
		else if (currentMapStage == 2) {
			isCollisionDetected2 = true;
		}
		else if (currentMapStage == 3) {
			isCollisionDetected3 = true;
		}
		else if (currentMapStage == 4) {
			isCollisionDetected4 = true;
		}

		currentMapStage++;
		spherePosition.x = 0.0f;
		spherePosition.y = 0.55f;
		spherePosition.z = 0.0f;

		if (currentMapStage > 4) {
			currentMapStage = 1; // ������ �������� ���� ó������ ���ư�
		}
		//resetBallPosition(); // �� ��ġ �ʱ�ȭ
	}
}

// ��ǥ ��ġ ��� ���� �Լ�
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