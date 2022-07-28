#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
using namespace std;

// User Manual Text
void Welcome()
{
	cout << "*****************************************************************\n";
	cout << "|                         User Manual                           |\n";
	cout << "*****************************************************************\n";
	cout << "| Press:                                                        |\n";
	cout << "|   <W>,<A>,<S>,<D> => move world                               |\n";
	cout << "|   <Q>, <E>        => move up, down                            |\n";
	cout << "|   ESC             => exit                                     |\n";
	cout << "|                                                               |\n";
	cout << "| Mouse Move        => rotate world                             |\n";
	cout << "|                                                               |\n";
	cout << "*****************************************************************\n";
	cout << "|                      H A V E   F U N  !!!                     |\n";
	cout << "*****************************************************************\n";
}

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void do_movement();
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Window dimensions
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Camera
float camUpDown = 0.5;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.5f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat yaw = -90.0f; // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];

// Deltatime
GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f;   // Time of last frame

// Light attributes
glm::vec3 lightPos2(6.0f, 0.0f, 0.0f);

// The MAIN function, from here we start the application and run the game loop

GLfloat coneVert[];

int k = 0;



int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	GLFWwindow* window;

	Welcome();

	if (!glfwInit()) 
		return -1;

	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, HEIGHT, HEIGHT);

	glEnable(GL_DEPTH_TEST);

	GLuint shaderProgram = initShader("vert.glsl", "frag.glsl");
	GLuint lampshader = initShader("lampvert.glsl", "lampfrag.glsl");

	GLfloat square[] = {

		// front

		-0.06f,   0.5f, -0.1f,  1.0f, 0.0f, 0.0f,// a

		-0.06f, -0.5f, -0.1f,  1.0f, 0.0f, 0.0f,//b

		0.12f,  -0.5f, -0.1f,  1.0f, 0.0f, 0.0f, // c

		0.12f,  -0.5f,-0.1f,  1.0f, 0.0f, 0.0f, // c

		0.12f,  0.5f, -0.1f,  1.0f, 0.0f, 0.0f, // d

		-0.06f,  0.5f, -0.1f,  1.0f, 0.0f, 0.0f,// a

		// back

		-0.06f,   0.5f, -0.3f,  1.0f, 0.0f, 0.0f,// e

		-0.06f, -0.5f, -0.3f,  1.0f, 0.0f, 0.0f,//f

		0.12f,  -0.5f, -0.3f,  1.0f, 0.0f, 0.0f, // h

		0.12f,  -0.5f, -0.3f,  1.0f, 0.0f, 0.0f, // h

		0.12f,  0.5f, -0.3f,  1.0f, 0.0f, 0.0f, // g

		-0.06f,   0.5f, -0.3f,  1.0f, 0.0f, 0.0f,// e

		// top

		-0.06f,   0.5f, -0.1f,  1.0f, 0.0f, 0.0f,// a

		-0.06f,   0.5f, -0.3f,  1.0f, 0.0f, 0.0f,// e

		0.12f,  0.5f, -0.1f,  1.0f, 0.0f, 0.0f, // d

		0.12f,  0.5f, -0.1f,  1.0f, 0.0f, 0.0f, // d

		0.12f,  0.5f, -0.3f,  1.0f, 0.0f, 0.0f, // g

		-0.06f,   0.5f, -0.3f,  1.0f, 0.0f, 0.0f,// e

		//bottom

		-0.06f, -0.5f, -0.1f,  1.0f, 0.0f, 0.0f,//b

		-0.06f, -0.5f, -0.3f,  1.0f, 0.0f, 0.0f,//f

		0.12f,  -0.5f, -0.1f,  1.0f, 0.0f, 0.0f, // c

		0.12f,  -0.5f, -0.1f,  1.0f, 0.0f, 0.0f, // c

		0.12f,  -0.5f, -0.3f,  1.0f, 0.0f, 0.0f, // h

		-0.06f, -0.5f, -0.3f,  1.0f, 0.0f, 0.0f,//f

		// left side

		-0.06f,   0.5f,-0.3f,  1.0f, 0.0f, 0.0f,// a

		-0.06f,   0.5f, -0.3f,  1.0f, 0.0f, 0.0f,// e

		-0.06f, -0.5f, -0.1f,  1.0f, 0.0f, 0.0f,//b

		-0.06f, -0.5f, -0.1f,  1.0f, 0.0f, 0.0f,//b

		-0.06f, -0.5f, -0.3f,  1.0f, 0.0f, 0.0f,//f

		-0.06f,   0.5f, -0.3f,  1.0f, 0.0f, 0.0f,// e

		// right side

		0.12f,  0.5f, -0.1f,  1.0f, 0.0f, 0.0f, // d

		0.12f,  0.5f, -0.3f,  1.0f, 0.0f, 0.0f, // g

		0.12f,  -0.5f, -0.1f,  1.0f, 0.0f, 0.0f, // c

		0.12f,  -0.5f, -0.1f,  1.0f, 0.0f, 0.0f, // c

		0.12f,  -0.5f, -0.3f,  1.0f, 0.0f, 0.0f, // h

		0.12f,  0.5f, -0.3f,  1.0f, 0.0f, 0.0f, // g

	};

	GLfloat Surface[] =
	{
		-10.0,-2.0, 10.5, 1.0f, 0.0f, 0.0f,
		 10.0,-2.0, 10.5, 1.0f, 0.0f, 0.0f,
		 10.0,-2.0, -10.0,1.0f, 0.0f, 0.0f,

		 10.0,-2.0, -10.0, 1.0f, 0.0f, 0.0f,
		-10.0,-2.0, -10.0, 1.0f, 0.0f, 0.0f,
		-10.0,-2.0,  10.5, 1.0f, 0.0f, 0.0f
	};

	GLfloat cones[] = 
	{
		0.03f,  0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	0.1,  1.0f, 0.0f, 0.0f,
		//TOP SIDE
		0.03f,  1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	0.1,  1.0f, 0.0f, 0.0f,

		0.03f,   0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.6,  1.0f, 0.0f, 0.0f,
		//TOP SIDE
		0.03f,   1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.6,  1.0f, 0.0f, 0.0f,


		0.03f,   0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.6,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	-0.9,  1.0f, 0.0f, 0.0f,

		//TOP SIDE 
		0.03f,   1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.6,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	-0.9,  1.0f, 0.0f, 0.0f,



		
		//
		0.03f,   0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	0.1,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.2,  1.0f, 0.0f, 0.0f,
		//TOP SIDE 
		0.03f,   1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	0.1,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.2,  1.0f, 0.0f, 0.0f,
		
		0.03f,   0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.2,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.6,  1.0f, 0.0f, 0.0f,
		//TOP SIDE 
		0.03f,   1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.2,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.6,  1.0f, 0.0f, 0.0f,

		0.03f,   0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	-0.9,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.6,  1.0f, 0.0f, 0.0f,
		//TOP SIDE
		0.03f,  1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	-0.9,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.6,  1.0f, 0.0f, 0.0f,
	};

	GLfloat lines[] = 
	{
		0.03f,  0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,

		0.03f,  1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		
		0.03f,  0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	0.1,  1.0f, 0.0f, 0.0f,

		0.03f,  1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	0.1,  1.0f, 0.0f, 0.0f,
		//C1
		-0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.6,  1.0f, 0.0f, 0.0f,

		-0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	0.1,  1.0f, 0.0f, 0.0f,
		//---
		0.03f,  1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		-0.5,	1.0,	-0.6,  1.0f, 0.0f, 0.0f,

		0.03f,   0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		-0.5,	 1.0,	-0.6,  1.0f, 0.0f, 0.0f,

		0.03f,   0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	-0.9,  1.0f, 0.0f, 0.0f,

		0.03f,   1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	-0.9,  1.0f, 0.0f, 0.0f,
		//C2
		-0.5,	1.0,	-0.6,  1.0f, 0.0f, 0.0f,
		0.03,	1.0,	-0.9,  1.0f, 0.0f, 0.0f,
		//--

		0.03f,   1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.2,  1.0f, 0.0f, 0.0f,

		0.03f,   0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.2,  1.0f, 0.0f, 0.0f,

		0.03f,   1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.6,  1.0f, 0.0f, 0.0f,

		0.03f,  0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.6,  1.0f, 0.0f, 0.0f,
		//C3
		0.03,	1.0,   -0.9,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.6,  1.0f, 0.0f, 0.0f,

		0.03,	1.0,	0.1,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.2,  1.0f, 0.0f, 0.0f,

		0.51,	1.0,   -0.2,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.6,  1.0f, 0.0f, 0.0f,
		//--
		0.03f,  1.2f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.6,  1.0f, 0.0f, 0.0f,

		0.03f,  0.5f, -0.4f,  1.0f, 0.0f, 0.0f,
		0.51,	1.0,   -0.6,  1.0f, 0.0f, 0.0f,

	};


	GLfloat cones2[] =
	{
		 0.03f,  -0.5f, -0.2f,  1.0f, 0.0f, 0.0f,
		-0.6,	-0.7,	-0.2,  1.0f, 0.0f, 0.0f,
		-0.5,	-0.7,	0.1,  1.0f, 0.0f, 0.0f,


		0.03f,   -0.5f, -0.2f,  1.0f, 0.0f, 0.0f,
		-0.6,	-0.7,	-0.2,  1.0f, 0.0f, 0.0f,
		-0.5,	-0.7,	-0.4,  1.0f, 0.0f, 0.0f,

		//Front Side of Cone 
		0.03f,   -0.5f, -0.2f,  1.0f, 0.0f, 0.0f,
		-0.5,	-0.7,	0.1,  1.0f, 0.0f, 0.0f,
		0.03,	-0.7,	0.4,  1.0f, 0.0f, 0.0f,

		//Back Side of Cone from Camera Perspective
		0.03f,   -0.5f, -0.2f,  1.0f, 0.0f, 0.0f,
		-0.5,	-0.7,	-0.4,  1.0f, 0.0f, 0.0f,
		0.01,	-0.7,	-0.6,  1.0f, 0.0f, 0.0f,

		//
		0.03f,   -0.5f, -0.2f,  1.0f, 0.0f, 0.0f,
		0.03,	-0.7,	0.4,  1.0f, 0.0f, 0.0f,
		0.5,	-0.7,	0.1,  1.0f, 0.0f, 0.0f,

		////
		0.03f,   -0.5f, -0.2f,  1.0f, 0.0f, 0.0f,
		0.5,	-0.7,	0.1,  1.0f, 0.0f, 0.0f,
		0.6,	-0.7,   -0.2,  1.0f, 0.0f, 0.0f,

		//Back Side right most
		0.03f,  -0.5f, -0.2f,  1.0f, 0.0f, 0.0f,
		0.02,	-0.7,   -0.6,  1.0f, 0.0f, 0.0f,
		0.36,	-0.7,   -0.4,  1.0f, 0.0f, 0.0f,


		0.03f,  -0.5f, -0.2f,  1.0f, 0.0f, 0.0f,
		0.36,	-0.7f,   -0.4,  1.0f, 0.0f, 0.0f,
		0.59,	-0.7f,   -0.2,  1.0f, 0.0f, 0.0f,

	};

	GLfloat arrow[] = {
		//Front 
		0.0,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		0.5,	1.2,	-0.2,  1.0f, 0.0f, 0.0f,

		0.5,	1.2,	-0.2,  1.0f, 0.0f, 0.0f,
		0.0,	1.2,	-0.2,  1.0f, 0.0f, 0.0f,
		0.0,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		//Back 
		0.0,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
		0.5,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
		0.5,	1.2,	0.6,  1.0f, 0.0f, 0.0f,

		0.5,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		0.0,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		0.0,	1.0,	0.6,  1.0f, 0.0f, 0.0f,

		//Back and Front 
		0.0,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		0.0,	1.2,	-0.2,  1.0f, 0.0f, 0.0f,
		0.0,	1.2,	0.6,  1.0f, 0.0f, 0.0f,

		0.0,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		0.0,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
		0.0,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,

		0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		0.5,	1.2,	-0.2,  1.0f, 0.0f, 0.0f,
		0.5,	1.2,	0.6,  1.0f, 0.0f, 0.0f,

		0.5,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		0.5,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
		0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,

		//TOP and BOTTOM
		0.0,	1.2,	-0.2,  1.0f, 0.0f, 0.0f,
		0.0,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		0.5,	1.2,	0.6,  1.0f, 0.0f, 0.0f,

		0.5,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		0.5,	1.2,	-0.2,  1.0f, 0.0f, 0.0f,
		0.0,	1.2,	-0.2,  1.0f, 0.0f, 0.0f,

		0.0,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		0.0,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
		0.5,	1.0,	0.6,  1.0f, 0.0f, 0.0f,

		0.5,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
		0.5,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,
		0.0,	1.0,	-0.2,  1.0f, 0.0f, 0.0f,


		//Arrow
		-0.2,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
		 0.7,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
		 0.7,	1.2,	0.6,  1.0f, 0.0f, 0.0f,

		 0.7,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		-0.2,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		-0.2,	1.0,	0.6,  1.0f, 0.0f, 0.0f,

		//Left Side 
		-0.2,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
		-0.2,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		0.25,	1.2,	0.85,  1.0f, 0.0f, 0.0f,

		0.25,	1.2,	0.85,  1.0f, 0.0f, 0.0f,
		0.25,	1.0,	0.85,  1.0f, 0.0f, 0.0f,
		-0.2,	1.0,	0.6,  1.0f, 0.0f, 0.0f,

		//Right Side 
		 0.7,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
		 0.7,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		0.25,	1.2,	0.85,  1.0f, 0.0f, 0.0f,

		0.25,	1.2,	0.85,  1.0f, 0.0f, 0.0f,
		0.25,	1.0,	0.85,  1.0f, 0.0f, 0.0f,
		 0.7,	1.0,	0.6,  1.0f, 0.0f, 0.0f,

		 //TOP Side Arrow 
		 0.7,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		-0.2,	1.2,	0.6,  1.0f, 0.0f, 0.0f,
		0.25,	1.2,	0.85,  1.0f, 0.0f, 0.0f,

		//Bottom Side Arrow 
		0.7,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
	   -0.2,	1.0,	0.6,  1.0f, 0.0f, 0.0f,
	   0.25,	1.0,	0.85,  1.0f, 0.0f, 0.0f,

	};

	GLuint VBO[8], VAO[8];
	glGenVertexArrays(8, VAO);
	glGenBuffers(8, VBO);
	//square
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

	//glBindVertexArray(containerVAO);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);


	//Cone 
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cones), cones, GL_STATIC_DRAW);

	//glBindVertexArray(containerVAO);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);


	//Cone 
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);

	//glBindVertexArray(containerVAO);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);


	//Surface 
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Surface), Surface, GL_STATIC_DRAW);

	//glBindVertexArray(containerVAO);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//Arrow
	glBindVertexArray(VAO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(arrow), arrow, GL_STATIC_DRAW);

	//glBindVertexArray(containerVAO);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	
	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the normal vectors
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		do_movement();
		

		glClearColor(0.0, 0.8, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
		
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);



		// Create camera transformations
		glm::mat4 view;
		glm::mat4 projection;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		


		glm::mat4 model;

		glBindVertexArray(VAO[1]);
		glm::vec3 blue1D(0.0f, 0.0f, 0.5f);
		model = glm::translate(model, blue1D);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(objectColorLoc, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glBindVertexArray(VAO[2]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);
		glLineWidth(2.0);
		glDrawArrays(GL_LINES, 0, 36);
		glBindVertexArray(0);

		glBindVertexArray(VAO[3]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(objectColorLoc, 0.0f, 1.0f, 0.0f);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glBindVertexArray(VAO[4]);
		glm::vec3 yellowA(-0.8f, 0.0f, 0.5f);
		model = glm::rotate(model, -0.6f, glm::vec3(0, 1, 0));
		model = glm::translate(model, yellowA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
		glDrawArrays(GL_TRIANGLES, 0, 104);
		glBindVertexArray(0);

		glBindVertexArray(VAO[1]);
		glm::vec3 blueD(0.5f, 0.0f, 2.0f);
		model = glm::translate(model, blueD);
		glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glBindVertexArray(VAO[2]);
		glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_LINES, 0, 36);
		glBindVertexArray(0);

		glBindVertexArray(VAO[4]);
		glm::vec3 blueA(0.0f, 0.0f, 0.8f);
		model = glm::rotate(model, 1.0f, glm::vec3(0, 1, 0));
		model = glm::translate(model, blueA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(objectColorLoc, 0.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 104);
		glBindVertexArray(0);

		glBindVertexArray(VAO[1]);
		glm::vec3 cyanD(0.5f, 0.0f, 2.0f);
		model = glm::translate(model, cyanD);
		glUniform3f(objectColorLoc, 0.0f, 1.0f, 1.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glBindVertexArray(VAO[2]);
		glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_LINES, 0, 36);
		glBindVertexArray(0);

		glBindVertexArray(VAO[4]);
		glm::vec3 cyanA(0.0f, 0.0f, 1.0f);
		model = glm::rotate(model, 1.2f, glm::vec3(0, 1, 0));
		model = glm::translate(model, cyanA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(objectColorLoc, 1.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 104);
		glBindVertexArray(0);

		glBindVertexArray(VAO[1]);
		glm::vec3 purpleD(0.5f, 0.0f, 2.0f);
		model = glm::translate(model, purpleD);
		glUniform3f(objectColorLoc, 1.0f, 0.0f, 1.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glBindVertexArray(VAO[2]);
		glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_LINES, 0, 36);
		glBindVertexArray(0);

		glBindVertexArray(VAO[4]);
		glm::vec3 purpleA(0.0f, 0.0f, 0.8f);
		model = glm::rotate(model, 1.0f, glm::vec3(0, 1, 0));
		model = glm::translate(model, purpleA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
		glDrawArrays(GL_TRIANGLES, 0, 104);
		glBindVertexArray(0);

		glBindVertexArray(VAO[1]);
		glm::vec3 redD(0.3f, 0.0f, 2.0f);
		model = glm::translate(model, redD);
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 0.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glBindVertexArray(VAO[2]);
		glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_LINES, 0, 36);
		glBindVertexArray(0);

		glBindVertexArray(VAO[4]);
		glm::vec3 redA(0.0f, 0.0f, 0.9f);
		model = glm::rotate(model, 1.6f, glm::vec3(0, 1, 0));
		model = glm::translate(model, redA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(objectColorLoc, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 104);
		glBindVertexArray(0);
	
		float material[] = { 1,0,0,0 };

		// Also draw the lamp object, again binding the appropriate shader
		glUseProgram(lampshader);
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampshader, "model");
		viewLoc = glGetUniformLocation(lampshader, "view");
		projLoc = glGetUniformLocation(lampshader, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4();

		model = glm::translate(model, lightPos2);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

float speed = 5.0f;
void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = speed * deltaTime;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_Q])
		cameraPos += cameraSpeed * cameraUp;
	if (keys[GLFW_KEY_E])
		cameraPos -= cameraSpeed * cameraUp;
	if (keys[GLFW_KEY_P])
		if (k == 0) {
			k = 1;
		}
		else
		{
			k = 0;
		}
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (GLfloat)xpos;
		lastY = (GLfloat)ypos;
		firstMouse = false;
	}

	GLfloat xoffset = (GLfloat)xpos - lastX;
	GLfloat yoffset = lastY - (GLfloat)ypos; // Reversed since y-coordinates go from bottom to left
	lastX = (GLfloat)xpos;
	lastY = (GLfloat)ypos;

	GLfloat sensitivity = (GLfloat)0.05; // Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		speed += 1.0;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		speed -= 1.0;
	}
}

