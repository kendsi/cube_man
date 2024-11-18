//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "initShader.h"
#include "sphere.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "texture.hpp"

enum eShadeMode { NO_LIGHT, GOURAUD, PHONG, NUM_LIGHT_MODE };

glm::mat4 projectMat;
glm::mat4 viewMat;
glm::mat4 modelMat = glm::mat4(1.0f);

int shadeMode = PHONG;
int isTexture = true;

GLuint projectMatrixID;
GLuint viewMatrixID;
GLuint modelMatrixID;
GLuint shadeModeID;
GLuint textureModeID;

float upperArmRotation = 0.0f;
float lowerArmRotation = 0.0f;
float upperLegRotation = 0.0f;
float lowerLegRotation = 0.0f;
float elapsedTime = 0.0f;

typedef glm::vec4 point4;
typedef glm::vec4 color4;
typedef glm::vec4 normal4;
typedef glm::vec2 texCoord2;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];
normal4 normals[NumVertices];		// 추가된 normal vector 배열
texCoord2 texCoords[NumVertices];	// 추가된 texCoord 배열

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA colors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),		// black
	color4(0.0, 1.0, 1.0, 1.0),		// cyan
	color4(1.0, 0.0, 1.0, 1.0),		// magenta
	color4(1.0, 1.0, 0.0, 1.0),		// yellow
	color4(1.0, 0.0, 0.0, 1.0),		// red
	color4(0.0, 1.0, 0.0, 1.0),		// green
	color4(0.0, 0.0, 1.0, 1.0),		// blue
	color4(1.0, 1.0, 1.0, 1.0)		// white
};

// cube 각 면의 normal vector 배열
normal4 face_normals[6] = {
	normal4(0.0,  0.0,  1.0, 0.0),	// 앞
	normal4(0.0,  0.0, -1.0, 0.0),	// 뒤
	normal4(-1.0,  0.0,  0.0, 0.0), // 좌
	normal4(1.0,  0.0,  0.0, 0.0),	// 우
	normal4(0.0,  1.0,  0.0, 0.0),	// 상
	normal4(0.0, -1.0,  0.0, 0.0)	// 하
};

// cube 각 면의 texCoord 배열
texCoord2 face_texCoords[4] = {
	texCoord2(0.0f, 0.0f),			// 좌하단
	texCoord2(1.0f, 0.0f),			// 우하단
	texCoord2(1.0f, 1.0f),			// 우상단
	texCoord2(0.0f, 1.0f)			// 좌상단
};

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
//void quad(int a, int b, int c, int d, int face) {
//	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[0]; Index++;
//	colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[1]; Index++;
//	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[2]; Index++;
//	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[0]; Index++;
//	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[2]; Index++;
//	colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[3]; Index++;
//}

int Index = 0;
void quad(int a, int b, int c, int d, int face) {
	points[Index] = vertices[a]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[0]; Index++;
	points[Index] = vertices[b]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[1]; Index++;
	points[Index] = vertices[c]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[2]; Index++;
	points[Index] = vertices[a]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[0]; Index++;
	points[Index] = vertices[c]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[2]; Index++;
	points[Index] = vertices[d]; normals[Index] = face_normals[face]; texCoords[Index] = face_texCoords[3]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void colorcube() {
	quad(1, 0, 3, 2, 0); // 앞
	quad(4, 5, 6, 7, 1); // 뒤
	quad(0, 4, 7, 3, 2); // 좌
	quad(5, 1, 2, 6, 3); // 우
	quad(1, 5, 4, 0, 4); // 상
	quad(3, 7, 6, 2, 5); // 하
}

//----------------------------------------------------------------------------
struct Keyframe {
	float time;         // 키프레임 시간 (초 단위)
	float rotationAngle;     // 해당 시간에서의 회전 각도
};

//----------------------------------------------------------------------------
const int numKeyframes = 4;
const float cycleTime = 2000.0f;

// 팔 하부, 다리 하부의 키프레임 데이터
Keyframe upperArmKeyframes[numKeyframes] = {
	{ 0.0f, glm::radians(-60.0f) },
	{ cycleTime / 3, glm::radians(-30.0f) },
	{ cycleTime * 2 / 3, glm::radians(0.0f) },
	{ cycleTime, glm::radians(30.0f) }
};

Keyframe lowerArmKeyframes[numKeyframes] = {
	{ 0.0f, glm::radians(15.0f) },
	{ cycleTime / 3, glm::radians(30.0f) },
	{ cycleTime * 2 / 3, glm::radians(90.0f) },
	{ cycleTime, glm::radians(110.0f) }
};

Keyframe upperLegKeyframes[numKeyframes] = {
	{ 0.0f, glm::radians(45.0f) },
	{ cycleTime / 3, glm::radians(15.0f) },
	{ cycleTime * 2 / 3, glm::radians(0.0f) },
	{ cycleTime, glm::radians(-15.0f) }
};

Keyframe lowerLegKeyframes[numKeyframes] = {
	{ 0.0f, glm::radians(30.0f) },
	{ cycleTime / 3, glm::radians(-30.0f) },
	{ cycleTime * 2 / 3, glm::radians(-90.0f) },
	{ cycleTime, glm::radians(-60.0f) }
};

//----------------------------------------------------------------------------

Sphere sphere(30, 30);
GLuint cubeVao, sphereVAO;

// OpenGL initialization
void init()
{
	// Load shaders and use the resulting shader program
	GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	// cube 초기화
	colorcube();

	// Create a vertex array object
	glGenVertexArrays(1, &cubeVao);
	glBindVertexArray(cubeVao);

	// Create and initialize a buffer object
	GLuint cubeBuffer;
	glGenBuffers(1, &cubeBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);

	// cube의 vertex, normal, texCoord size
	int cubeVertSize = sizeof(points);
	int cubeNormalSize = sizeof(normals);
	int cubeTexCoordSize = sizeof(texCoords);

	glBufferData(GL_ARRAY_BUFFER, cubeVertSize + cubeNormalSize + cubeTexCoordSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, cubeVertSize, points);
	glBufferSubData(GL_ARRAY_BUFFER, cubeVertSize, cubeNormalSize, normals);
	glBufferSubData(GL_ARRAY_BUFFER, cubeVertSize + cubeNormalSize, cubeTexCoordSize, texCoords);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cubeVertSize));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cubeVertSize + cubeNormalSize));

	// Sphere 초기화
	glGenVertexArrays(1, &sphereVAO);
	glBindVertexArray(sphereVAO);

	GLuint sphereBuffer;
	glGenBuffers(1, &sphereBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphereBuffer);

	int sphereVertSize = sizeof(sphere.verts[0]) * sphere.verts.size();
	int sphereNormalSize = sizeof(sphere.normals[0]) * sphere.normals.size();
	int sphereTexCoordSize = sizeof(sphere.texCoords[0]) * sphere.texCoords.size();

	glBufferData(GL_ARRAY_BUFFER, sphereVertSize + sphereNormalSize + sphereTexCoordSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sphereVertSize, sphere.verts.data());
	glBufferSubData(GL_ARRAY_BUFFER, sphereVertSize, sphereNormalSize, sphere.normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, sphereVertSize + sphereNormalSize, sphereTexCoordSize, sphere.texCoords.data());

	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sphereVertSize));

	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sphereVertSize + sphereNormalSize));

	projectMatrixID = glGetUniformLocation(program, "mProject");
	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);

	viewMatrixID = glGetUniformLocation(program, "mView");
	viewMat = glm::lookAt(glm::vec3(0.3, 1.0, 2.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);

	modelMatrixID = glGetUniformLocation(program, "mModel");
	modelMat = glm::mat4(1.0f);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);

	shadeModeID = glGetUniformLocation(program, "shadeMode");
	glUniform1i(shadeModeID, shadeMode);

	textureModeID = glGetUniformLocation(program, "isTexture");
	glUniform1i(textureModeID, isTexture);

	GLuint texture = loadBMP_custom("texture.bmp");
	GLuint textureID = glGetUniformLocation(program, "manTexture");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureID, 0);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// drawCube 함수
void drawCube() {
	glBindVertexArray(cubeVao); // cube VAO 바인딩
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

// drawSphere 함수
void drawSphere() {
	glBindVertexArray(sphereVAO); // sphere VAO 바인딩
	glDrawArrays(GL_TRIANGLES, 0, sphere.verts.size());
}

//----------------------------------------------------------------------------

float interpolateKeyframes(const Keyframe keyframes[], int numKeyframes, float time) {
	if (time < 0.0f) {
		time = -time;
	}
	else if (time > cycleTime) {
		time = 2 * cycleTime - time;
	}

	for (int i = 0; i < numKeyframes - 1; i++) {
		const Keyframe& kf1 = keyframes[i];
		const Keyframe& kf2 = keyframes[i + 1];

		if (time >= kf1.time && time <= kf2.time) {
			float t = (time - kf1.time) / (kf2.time - kf1.time); // 0.0 ~ 1.0 사이 값
			return glm::mix(kf1.rotationAngle, kf2.rotationAngle, t);
		}
	}
	return 0.0f; // 기본값
}

void drawMan()
{
	glm::mat4 cubeMat, sphereMat;
	glm::vec3 armPos[2], legPos[2];


	// limb positions
	armPos[0] = glm::vec3(-0.5, 0.2, 0); // left upper arm
	armPos[1] = glm::vec3(0.5, 0.2, 0);  // right upper arm

	legPos[0] = glm::vec3(-0.25, -0.8, 0); // left upper leg
	legPos[1] = glm::vec3(0.25, -0.8, 0);  // right upper leg

	// man head
	sphereMat = glm::translate(modelMat, glm::vec3(0, 0.7, 0));
	sphereMat = glm::scale(sphereMat, glm::vec3(0.23, 0.23, 0.23));
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &sphereMat[0][0]);
	drawSphere();

	// man body
	cubeMat = glm::scale(modelMat, glm::vec3(0.7, 1, 0.5));
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &cubeMat[0][0]);
	drawCube();


	// man limbs
	for (int i = 0; i < 2; ++i) {

		if (i == 0) {
			upperArmRotation = interpolateKeyframes(upperArmKeyframes, numKeyframes, elapsedTime);
			lowerArmRotation = interpolateKeyframes(lowerArmKeyframes, numKeyframes, elapsedTime);
			upperLegRotation = interpolateKeyframes(upperLegKeyframes, numKeyframes, elapsedTime);
			lowerLegRotation = interpolateKeyframes(lowerLegKeyframes, numKeyframes, elapsedTime);
		}
		else {
			upperArmRotation = interpolateKeyframes(upperArmKeyframes, numKeyframes, cycleTime - elapsedTime);
			lowerArmRotation = interpolateKeyframes(lowerArmKeyframes, numKeyframes, cycleTime - elapsedTime);
			upperLegRotation = interpolateKeyframes(upperLegKeyframes, numKeyframes, cycleTime - elapsedTime);
			lowerLegRotation = interpolateKeyframes(lowerLegKeyframes, numKeyframes, cycleTime - elapsedTime);
		}

		// 팔 상부 회전
		cubeMat = glm::translate(modelMat, armPos[i]);
		cubeMat = glm::translate(cubeMat, glm::vec3(0.0, 0.2, 0.0));
		cubeMat = glm::rotate(cubeMat, upperArmRotation, glm::vec3(1, 0, 0)); // 키프레임 보간 값 사용
		cubeMat = glm::translate(cubeMat, glm::vec3(0.0, -0.2, 0.0));
		cubeMat = glm::scale(cubeMat, glm::vec3(0.2, 0.4, 0.2));
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &cubeMat[0][0]);
		drawCube();

		// 팔 하부 회전
		glm::vec4 upperArmBottomPos = cubeMat * glm::vec4(0.0, -0.4, 0.0, 1.0);
		cubeMat = glm::translate(modelMat, glm::vec3(upperArmBottomPos));
		cubeMat = glm::rotate(cubeMat, lowerArmRotation, glm::vec3(1, 0, 0)); // 키프레임 보간 값 사용
		cubeMat = glm::translate(cubeMat, glm::vec3(0.0, -0.2, 0.0));
		cubeMat = glm::scale(cubeMat, glm::vec3(0.2, 0.4, 0.2));
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &cubeMat[0][0]);
		drawCube();

		// 다리 상부 회전
		cubeMat = glm::translate(modelMat, legPos[i]);
		cubeMat = glm::translate(cubeMat, glm::vec3(0.0, 0.2, 0.0));
		cubeMat = glm::rotate(cubeMat, upperLegRotation, glm::vec3(1, 0, 0)); // 키프레임 보간 값 사용
		cubeMat = glm::translate(cubeMat, glm::vec3(0.0, -0.2, 0.0));
		cubeMat = glm::scale(cubeMat, glm::vec3(0.2, 0.4, 0.2));
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &cubeMat[0][0]);
		drawCube();

		// 다리 하부 회전
		glm::vec4 upperLegBottomPos = cubeMat * glm::vec4(0.0, -0.4, 0.0, 1.0);
		cubeMat = glm::translate(modelMat, glm::vec3(upperLegBottomPos));
		cubeMat = glm::rotate(cubeMat, lowerLegRotation, glm::vec3(1, 0, 0)); // 키프레임 보간 값 사용
		cubeMat = glm::translate(cubeMat, glm::vec3(0.0, -0.2, 0.0));
		cubeMat = glm::scale(cubeMat, glm::vec3(0.2, 0.4, 0.2));
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &cubeMat[0][0]);
		drawCube();
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMan();
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void idle()
{
	static int startTime = glutGet(GLUT_ELAPSED_TIME);
	int currTime = glutGet(GLUT_ELAPSED_TIME);

	elapsedTime = fmod(abs(currTime - startTime), 2 * cycleTime);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 49:	// 1
		viewMat = glm::lookAt(glm::vec3(2, 0.5, -0.3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);
		glutPostRedisplay();
		break;
	case 50:	// 2
		viewMat = glm::lookAt(glm::vec3(0.3, 1.0, 2.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);
		glutPostRedisplay();
		break;
	case 51:	// 3
		viewMat = glm::lookAt(glm::vec3(-0.1, 0.5, -2.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);
		glutPostRedisplay();
		break;
	case 'l': case 'L':
		shadeMode = (++shadeMode % NUM_LIGHT_MODE);
		glUniform1i(shadeModeID, shadeMode);
		glutPostRedisplay();
		break;
	case 't': case 'T':
		isTexture = !isTexture;
		glUniform1i(textureModeID, isTexture);
		glutPostRedisplay();
		break;
	case 033:  // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

void resize(int w, int h)
{
	float ratio = (float)w / (float)h;
	glViewport(0, 0, w, h);

	projectMat = glm::perspective(glm::radians(65.0f), ratio, 0.1f, 100.0f);
	glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Running Man");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	// window size가 바뀔 떄마다 resize call
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
