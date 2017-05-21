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

class RenderEngine
{
private:
	ShaderProgram *basicShader;
	ShaderProgram *uiShader;
	ShaderProgram *fboShader;
	Mesh *mesh;
	Texture *texture;
	
	std::vector<Model *> models;

	ObjLoader objLoader;
	OrbitCamera camera;
	Grid uiGrid;
	ScreenQuad *fboScreenQuad;

	GLuint fboId;
	GLuint colorFBOTextureId;
	GLuint depthFBOTextureId;

	int window_width;
	int window_height;

public:
	RenderEngine(int window_width, int window_height);
	~RenderEngine();

	void initGL();
	void resize(int width, int height);
	void render();

	void initFrameBuffer(int texture_width, int texture_height);

	OrbitCamera &getCamera();
};

#endif