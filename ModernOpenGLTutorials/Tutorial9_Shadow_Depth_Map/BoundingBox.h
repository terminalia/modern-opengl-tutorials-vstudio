#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>


class BoundingBox
{
private:
	GLuint vbo_vertexdata;
	GLuint vbo_indices;
	GLuint vao;

	glm::vec3 color;
	glm::vec3 min_vertex;
	glm::vec3 max_vertex;
	glm::mat4 modelMatrix;

public:
	BoundingBox();
	BoundingBox(const glm::vec3 &min_vertex, const glm::vec3 &max_vertex, const glm::vec3 &color);
	BoundingBox(const BoundingBox &boundingBox);
	~BoundingBox();

	void cleanGLBuffers();
	void draw();

	glm::vec3 getMinVertex();
	glm::vec3 getMaxVertex();

};

#endif // BOUNDINGBOX_H
