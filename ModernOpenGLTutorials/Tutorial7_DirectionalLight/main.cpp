#include <iostream>
#include <sstream>
#include <exception>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#include "RenderEngine.h"

using namespace std;

//Set 1 for 60fps, 2 for 30fps, 0 for no v-sync
const int SWAP_INTERVAL = 0;
const unsigned int WINDOW_WIDTH = 1920;
const unsigned int WINDOW_HEIGHT = 1080;

//FPS
double lastTime;
double frameCount = 0;
bool mouseLeftPressed = false;
bool mouseRightPressed = false;
bool mouseMiddlePressed = false;
bool altPressed = false;
float mouseSensitivity = 0.009f;
double mousePositionx = 0;
double mousePositiony = 0;
int current_width = 0;
int current_height = 0;

GLFWwindow *window;

RenderEngine *renderEngine;

std::string title = "Tutorial7 - Directional Light";

void initGL()
{
	renderEngine->initGL();
}

void resize(GLFWwindow *window, int width, int height)
{
	current_width = width;
	current_height = height;
	renderEngine->resize(current_width, current_height);
}

void render()
{
	renderEngine->render();
}

void keyboardInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (mods == GLFW_MOD_ALT)
		{
			altPressed = true;
		}
	}

	if (action == GLFW_RELEASE)
	{
		altPressed = false;
	}
}

void mousePressed(GLFWwindow *window, int button, int action, int mod)
{
	if (action == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &mousePositionx, &mousePositiony);

		if (button == GLFW_MOUSE_BUTTON_1)
		{
			if (!altPressed)
			{
				renderEngine->castRay(mousePositionx, mousePositiony, current_width, current_height);
				renderEngine->findObjectInScreen(mousePositionx, mousePositiony, current_width, current_height);
			}
			mouseLeftPressed = true;
		}
		else if (button == GLFW_MOUSE_BUTTON_3)
		{
			glfwGetCursorPos(window, &mousePositionx, &mousePositiony);
			mouseMiddlePressed = true;
		}
		else if (button == GLFW_MOUSE_BUTTON_2)
		{
			glfwGetCursorPos(window, &mousePositionx, &mousePositiony);
			mouseRightPressed = true;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		mouseLeftPressed = false;
		mouseMiddlePressed = false;
		mouseRightPressed = false;
	}
}

void mouseMoved(GLFWwindow *window, double x, double y)
{
	float x_offset = x - mousePositionx;
	float y_offset = y - mousePositiony;

	if (mouseLeftPressed)
	{
		if (altPressed)
		{
			x_offset *= mouseSensitivity;
			y_offset *= mouseSensitivity;
			renderEngine->getCamera().rotate(x_offset, y_offset);
		}
		else
		{
			renderEngine->moveObject(mousePositionx, mousePositiony, current_width, current_height);
		}
	}
	else if (mouseMiddlePressed)
	{
		x_offset *= mouseSensitivity;
		y_offset *= mouseSensitivity * 0.1;
		renderEngine->getCamera().pan(x_offset, y_offset);
	}
	else if (mouseRightPressed)
	{
		y_offset *= mouseSensitivity * 10;
		renderEngine->getCamera().zoom(y_offset);
	}

	mousePositionx = x;
	mousePositiony = y;
}

void computeFPS()
{
	double currentTime = glfwGetTime();

	if ((currentTime - lastTime) > 1.0 || frameCount == 0)
	{
		double fps = (double)frameCount / (currentTime - lastTime);
		std::stringstream ss;
		ss << title.c_str() << " FPS: " << (int)fps;
		glfwSetWindowTitle(window, ss.str().c_str());
		lastTime = currentTime;
		frameCount = 0;
	}

	frameCount++;
}


void mainLoop()
{
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
		title.c_str(),
		NULL,
		NULL);

	if (!window)
	{
		cerr << "Failed creating GLFW window" << endl;
		return false;
	}

	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetKeyCallback(window, keyboardInput);
	glfwSetMouseButtonCallback(window, mousePressed);
	glfwSetCursorPosCallback(window, mouseMoved);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(SWAP_INTERVAL);

	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW" << endl;
		return false;
	}

	glfwGetFramebufferSize(window, &current_width, &current_height);

	renderEngine = new RenderEngine(current_width, current_height);

	initGL();
	resize(window, current_width, current_height);

	printContextInfos();

	return true;
}

void cleanUp()
{
	if (renderEngine)
		delete renderEngine;
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