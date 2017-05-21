#ifndef LINE_H
#define LINE_H

#include <GL/glew.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

class Line
{
private:
	GLuint vbo_vertexdata;
	GLuint vbo_indices;
	GLuint vao;

public:
	glm::vec3 start;
	glm::vec3 end;
	glm::vec3 color;

public:
	Line();
	Line(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 color);
	Line(const Line &line);
	~Line();

	void update(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color);
	void cleanGLBuffers();
	void draw();
};

#endif // LINE_H
