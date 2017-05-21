#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

class Mesh
{
private:
	std::string name;
	GLuint vbo_vertexdata;
	GLuint vbo_indices;
	GLuint vao;
	int numOfIndices;
	glm::vec3 min_vertex;
	glm::vec3 max_vertex;

public:
	Mesh();
	Mesh(const std::string &name, const std::vector<glm::vec3> &positions, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &texcoords, const std::vector<unsigned short> &indices);
	Mesh(const Mesh &mesh);
	~Mesh();

	void cleanGLBuffers();
	void draw();
	void setBBVisible(bool visible);

	bool isBBVisible();
	glm::vec3 getMinVertex();
	glm::vec3 getMaxVertex();
	std::string getName();
};

#endif // MESH_H
