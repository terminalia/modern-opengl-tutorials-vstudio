#include <iostream>
#include <sstream>
#include <exception>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#include "ObjLoader.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Texture.h"

using namespace std;

//Set 1 for 60fps, 2 for 30fps, 0 for no v-sync
const int SWAP_INTERVAL = 2;
const unsigned int WINDOW_WIDTH = 1024;
const unsigned int WINDOW_HEIGHT = 768;

//FPS
double lastTime;
double frameCount = 0;
std::string title = "Tutorial2 - OBJ Loader";

GLFWwindow *window;
ObjLoader objLoader;
ShaderProgram basicShader;
Mesh mesh;
Texture texture;

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;
glm::mat4 modelMatrix;

GLint projectionUniformLoc;
GLint viewUniformLoc;
GLint modelUniformLoc;



void initGL()
{
	glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	
	try {
		basicShader = ShaderProgram("Basic", "../Shaders/Tutorial2/basic.vert", "../Shaders/Tutorial2/basic.frag");
		objLoader.loadFile("../Assets/pig.obj");
		mesh = Mesh("PigMesh", objLoader.getPositions(), objLoader.getNormals(), objLoader.getTexCoords(), objLoader.getIndices());
		texture = Texture("PigTexture", GL_TEXTURE_2D, "../Assets/pig_diffuse_map.png");
	}
	catch (exception &ex) {
		std::cerr << ex.what() << std::endl;
	}

	projectionUniformLoc = glGetUniformLocation(basicShader.getProgram(), "uProjectionMat");
	viewUniformLoc = glGetUniformLocation(basicShader.getProgram(), "uViewMat");
	modelUniformLoc = glGetUniformLocation(basicShader.getProgram(), "uModelMat");
}

void resize(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
	float ratio = (float)width / (float)height;
	projectionMatrix = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);
	glUniformMatrix4fv(projectionUniformLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(basicShader.getProgram());

	modelMatrix = glm::mat4(1.0f);
	viewMatrix = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glUniformMatrix4fv(projectionUniformLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(viewUniformLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(modelUniformLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	texture.bind(GL_TEXTURE0);
	mesh.draw();
}

void keyboardInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;

	default:
		return;
	}
}

void computeFPS()
{
	double currentTime = glfwGetTime();

	if ((currentTime - lastTime) > 1.0 || frameCount == 0)
	{
		double fps = (double)frameCount / (currentTime - lastTime);
		std::stringstream ss;
		ss << title.c_str() << " FPS: " << (float)fps;
		glfwSetWindowTitle(window, ss.str().c_str());
		lastTime = currentTime;
		frameCount = 0;
	}

	frameCount++;
}


void mainLoop()
{
	initGL();

	while (!glfwWindowShouldClose(window))
	{
		render();
		glfwSwapBuffers(window);
		glfwPollEvents();
		computeFPS();
	}
}

void printContextInfos()
{
	cout << "OpenGL Version: " << glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR) << "." << glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR) << std::endl;
	GLint samples = 0;
	glGetIntegerv(GL_SAMPLES, &samples);
	cout << "Multi Samples: " << samples << std::endl;
}

bool initGLFW()
{
	if (!glfwInit())
	{
		cerr << "Failed initializing GLFW" << endl;
		return false;
	}

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_SAMPLES, 8);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Tutorial2 OBJ Loader",
		NULL,
		NULL);

	if (!window)
	{
		cerr << "Failed creating GLFW window" << endl;
		return false;
	}

	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetKeyCallback(window, keyboardInput);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(SWAP_INTERVAL);

	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW" << endl;
		return false;
	}

	int current_width, current_height;
	glfwGetFramebufferSize(window, &current_width, &current_height);

	resize(window, current_width, current_height);

	printContextInfos();

	return true;
}

void cleanUp()
{
	mesh.cleanGLBuffers();
	texture.cleanGLBuffers();
}

int main(int argc, char *argv[])
{
	if (initGLFW())
	{
		mainLoop();
	}

	cleanUp();
	glfwTerminate();

	return 0;
}