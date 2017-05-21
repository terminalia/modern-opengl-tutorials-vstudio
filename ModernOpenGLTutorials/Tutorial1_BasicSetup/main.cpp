#include <iostream>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW/glfw3.h>

using namespace std;

const int SWAP_INTERVAL = 0;
const unsigned int WINDOW_WIDTH = 1024;
const unsigned int WINDOW_HEIGHT = 768;
GLFWwindow *window;

void initGL()
{
	glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void resize(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Just a quick proof of concept! If we set SDL_GL_CONTEXT_PROFILE_MASK to SDL_GL_CONTEXT_PROFILE_COMPATIBILITY or 
	//we set Major/Minor version to a version < 3.3, we should see the triangle drawn with OpenGL 1.x/2.x deprecated code
	glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_TRIANGLES);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glEnd();
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

void mainLoop()
{
	initGL();

	while (!glfwWindowShouldClose(window))
	{
		render();
		glfwSwapBuffers(window);
		glfwPollEvents();
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

	//Only used by Mac OSX
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//Used with OpenGL 3.2+ profile to keep deprecated functionalities
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_SAMPLES, 8);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Tutorial1 Basic Setup",
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
}

int main(int argc, char *argv[])
{
	if (initGLFW())
	{
		mainLoop();
	}

	glfwTerminate();

	return 0;
}