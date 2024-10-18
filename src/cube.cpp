//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

glm::mat4 projectMat;
glm::mat4 viewMat;

GLuint pvmMatrixID;

float upperArmRotation = 0.0f;
float lowerArmRotation = 0.0f;
float upperLegRotation = 0.0f;
float lowerLegRotation = 0.0f;
float elapsedTime = 0.0f;

typedef glm::vec4  color4;
typedef glm::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

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
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(0.0, 1.0, 1.0, 1.0),   // cyan
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 1.0, 1.0, 1.0)  // white
};

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d];  Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
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

// OpenGL initialization
void
init()
{
	colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	pvmMatrixID = glGetUniformLocation(program, "mPVM");

	// vertical fov, aspect ratio, near plane, far plane
	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	// eye position, look at, up
	viewMat = glm::lookAt(glm::vec3(2, 0.5, -0.3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
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

void drawMan(glm::mat4 manMat)
{
	glm::mat4 modelMat, pvmMat;
	glm::vec3 armPos[2], legPos[2];


	// limb positions
	armPos[0] = glm::vec3(-0.5, 0.2, 0); // left upper arm
	armPos[1] = glm::vec3(0.5, 0.2, 0);  // right upper arm

	legPos[0] = glm::vec3(-0.25, -0.8, 0); // left upper leg
	legPos[1] = glm::vec3(0.25, -0.8, 0);  // right upper leg

	// man body
	modelMat = glm::scale(manMat, glm::vec3(0.7, 1, 0.5));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// man head
	modelMat = glm::translate(manMat, glm::vec3(0, 0.7, 0));
	modelMat = glm::scale(modelMat, glm::vec3(0.3, 0.3, 0.3));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

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
		modelMat = glm::translate(manMat, armPos[i]);
		modelMat = glm::translate(modelMat, glm::vec3(0.0, 0.2, 0.0));
		modelMat = glm::rotate(modelMat, upperArmRotation, glm::vec3(1, 0, 0)); // 키프레임 보간 값 사용
		modelMat = glm::translate(modelMat, glm::vec3(0.0, -0.2, 0.0));
		modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.4, 0.2));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);

		// 팔 하부 회전
		glm::vec4 upperArmBottomPos = modelMat * glm::vec4(0.0, -0.4, 0.0, 1.0);
		modelMat = glm::translate(manMat, glm::vec3(upperArmBottomPos));
		modelMat = glm::rotate(modelMat, lowerArmRotation, glm::vec3(1, 0, 0)); // 키프레임 보간 값 사용
		modelMat = glm::translate(modelMat, glm::vec3(0.0, -0.2, 0.0));
		modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.4, 0.2));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);

		// 다리 상부 회전
		modelMat = glm::translate(manMat, legPos[i]);
		modelMat = glm::translate(modelMat, glm::vec3(0.0, 0.2, 0.0));
		modelMat = glm::rotate(modelMat, upperLegRotation, glm::vec3(1, 0, 0)); // 키프레임 보간 값 사용
		modelMat = glm::translate(modelMat, glm::vec3(0.0, -0.2, 0.0));
		modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.4, 0.2));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);

		// 다리 하부 회전
		glm::vec4 upperLegBottomPos = modelMat * glm::vec4(0.0, -0.4, 0.0, 1.0);
		modelMat = glm::translate(manMat, glm::vec3(upperLegBottomPos));
		modelMat = glm::rotate(modelMat, lowerLegRotation, glm::vec3(1, 0, 0)); // 키프레임 보간 값 사용
		modelMat = glm::translate(modelMat, glm::vec3(0.0, -0.2, 0.0));
		modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.4, 0.2));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}
}

void display(void)
{
	glm::mat4 worldMat, pvmMat;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// other transformation matrices, rotation angle, rotation axis
	// == modeling transformation
	worldMat = glm::mat4(1.0f);

	drawMan(worldMat);

	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void idle()
{
	static int startTime = glutGet(GLUT_ELAPSED_TIME);
	int currTime = glutGet(GLUT_ELAPSED_TIME);

	elapsedTime = fmod(abs(currTime - startTime), 2*cycleTime);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 49:	// 1
		viewMat = glm::lookAt(glm::vec3(2, 0.5, -0.3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case 50:	// 2
		viewMat = glm::lookAt(glm::vec3(0.3, 1.0, 2.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case 51:	// 3
		viewMat = glm::lookAt(glm::vec3(-0.1, 0.5, -2.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
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

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Car");

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
