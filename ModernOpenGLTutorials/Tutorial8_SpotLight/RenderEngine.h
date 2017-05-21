#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include <iostream>
#include <sstream>
#include <exception>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#include "ObjLoader.h"
#include "ScreenQuad.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Texture.h"
#include "OrbitCamera.h"
#include "Grid.h"
#include "ScreenQuad.h"
#include "Model.h"
#include "Light.h"
#include "MathUtils.h"

class RenderEngine
{
private:
	ShaderProgram *spotLightShader;
	ShaderProgram *uiShader;
	ShaderProgram *fboShader;
	Mesh *pigMesh;
	Mesh *lightMesh;
	Mesh *groundMesh;
	Texture *pigTexture;
	Texture *groundTexture;
	
	std::vector<Model *> models;
	std::vector<Light *> lights;

	ObjLoader objLoader;
	OrbitCamera camera;
	Grid uiGrid;
	ScreenQuad *fboScreenQuad;

	GLuint fboId;
	GLuint colorFBOTextureId;
	GLuint depthFBOTextureId;

	glm::vec3 objectScreenPoint;
	glm::vec3 objectMouseOffset;
	std::string pickedObjectID;

	int window_width;
	int window_height;

public:
	RenderEngine(int window_width, int window_height);
	~RenderEngine();

	void initGL();
	void resize(int width, int height);
	void render();

	void initFrameBuffer(int texture_width, int texture_height);
	void castRay(int mouse_x, int mouse_y, int screen_width, int screen_height);
	void findObjectInScreen(int mouse_x, int mouse_y, int screen_width, int screen_height);
	void moveObject(int mouse_x, int mouse_y, int screen_width, int screen_height);

	OrbitCamera &getCamera();

	template<typename T>
	T findElement(const std::string &name, const std::vector<T> &vec);
};

#endif