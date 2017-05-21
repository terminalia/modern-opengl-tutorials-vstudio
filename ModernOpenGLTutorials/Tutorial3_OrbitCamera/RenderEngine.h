#ifndef RENDERENGINE_H
#define RENDERENGINE_H

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
#include "OrbitCamera.h"
#include "Grid.h"

class RenderEngine
{
private:
	ShaderProgram *basicShader;
	ShaderProgram *uiShader;
	Mesh *mesh;
	Texture *texture;
	ObjLoader objLoader;
	OrbitCamera camera;
	Grid uiGrid;


	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 modelMatrix;
	glm::mat4 normalMatrix;

	GLint projectionUniformLoc;
	GLint viewUniformLoc;
	GLint modelUniformLoc;
	GLint normalMatrixLoc;

public:
	RenderEngine();
	~RenderEngine();

	void initGL();
	void resize(int width, int height);
	void render();

	void updateUniformLoc(ShaderProgram *shader);
	void uploadUniformsToShader(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &modelMatrix, const glm::mat4 &normalMatrix);

	OrbitCamera &getCamera();
};

#endif