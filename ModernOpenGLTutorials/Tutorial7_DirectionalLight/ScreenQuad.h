#ifndef SCREENQUAD_H
#define SCREENQUAD_H

#include <GL/glew.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

class ScreenQuad
{
private:
	GLuint vbo_vertexdata;
	GLuint vao;

public:
	ScreenQuad();
	~ScreenQuad();

	void draw();
	void cleanGLBuffers();
};

#endif
