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

class RenderEngine
{
private:
	ShaderProgram *basicShader;
	ShaderProgram *uiShader;
	ShaderProgram *fboShader;
	Mesh *mesh;
	Texture *texture;
	
	ObjLoader objLoader;
	OrbitCamera camera;
	Grid uiGrid;
	ScreenQuad *fboScreenQuad;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 modelMatrix;
	glm::mat4 normalMatrix;

	GLuint fboId;
	GLuint colorFBOTextureId;
	GLuint depthFBOTextureId;

	GLint projectionUniformLoc;
	GLint viewUniformLoc;
	GLint modelUniformLoc;
	GLint normalMatrixLoc;

	int window_width;
	int window_height;

public:
	RenderEngine(int window_width, int window_height);
	~RenderEngine();

	void initGL();
	void resize(int width, int height);
	void render();

	void initFrameBuffer(int texture_width, int texture_height);

	void updateUniformLoc(ShaderProgram *shader);
	void uploadUniformsToShader(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &modelMatrix, const glm::mat4 &normalMatrix);

	OrbitCamera &getCamera();
};

#endif