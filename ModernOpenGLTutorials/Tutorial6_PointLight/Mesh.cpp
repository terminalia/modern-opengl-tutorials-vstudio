#include "mesh.h"

Mesh::Mesh()
{

}

Mesh::Mesh(const std::string &name, const std::vector<glm::vec3> &positions, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &texcoords, const std::vector<unsigned short> &indices)
{
	this->name = name;
	std::vector<GLfloat> data;
	for (unsigned int i = 0; i<positions.size(); i++)
	{
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);
		data.push_back(normals[i].x);
		data.push_back(normals[i].y);
		data.push_back(normals[i].z);
		data.push_back(texcoords[i].x);
		data.push_back(texcoords[i].y);
	}

	GLfloat *vertexData = new GLfloat[data.size()];

	for (unsigned int i = 0; i<data.size(); i++)
	{
		vertexData[i] = data[i];
	}

	unsigned short *indexData = new unsigned short[indices.size()];
	numOfIndices = indices.size();

	for (unsigned int i = 0; i<indices.size(); i++)
	{
		indexData[i] = indices[i];
	}

	glGenBuffers(1, &vbo_vertexdata);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexdata);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), vertexData, GL_STATIC_DRAW);


	glGenBuffers(1, &vbo_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * indices.size(), indexData, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexdata);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	float min_x = positions.at(0).x;
	float min_y = positions.at(0).y;
	float min_z = positions.at(0).z;
	float max_x = positions.at(0).x;
	float max_y = positions.at(0).y;
	float max_z = positions.at(0).z;


	for (unsigned int i = 0; i < positions.size(); i++) {
		if (positions[i].x < min_x) min_x = positions[i].x;
		if (positions[i].x > max_x) max_x = positions[i].x;
		if (positions[i].y < min_y) min_y = positions[i].y;
		if (positions[i].y > max_y) max_y = positions[i].y;
		if (positions[i].z < min_z) min_z = positions[i].z;
		if (positions[i].z > max_z) max_z = positions[i].z;
	}

	min_vertex = glm::vec3(min_x, min_y, min_z);
	max_vertex = glm::vec3(max_x, max_y, max_z);

	bb = BoundingBox(min_vertex, max_vertex, glm::vec3(0, 1, 0));
	bbVisible = false;

	delete[] vertexData;
	delete[] indexData;
}

Mesh::Mesh(const Mesh &mesh)
{
	name = mesh.name;
	vbo_vertexdata = mesh.vbo_vertexdata;
	vbo_indices = mesh.vbo_indices;
	vao = mesh.vao;
	numOfIndices = mesh.numOfIndices;
	min_vertex = mesh.min_vertex;
	max_vertex = mesh.max_vertex;
	bb = mesh.bb;
	bbVisible = mesh.bbVisible;
}

Mesh::~Mesh()
{

}

void Mesh::cleanGLBuffers()
{
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo_indices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo_vertexdata);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);
}

void Mesh::draw()
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	glDrawElements(GL_TRIANGLES, numOfIndices, GL_UNSIGNED_SHORT, 0);
}

void Mesh::drawBB()
{
	bb.draw();
}

void Mesh::setBBVisible(bool visible)
{
	bbVisible = visible;
}

bool Mesh::isBBVisible()
{
	return bbVisible;
}

glm::vec3 Mesh::getMinVertex()
{
	return min_vertex;
}

glm::vec3 Mesh::getMaxVertex()
{
	return max_vertex;
}

BoundingBox & Mesh::getBB()
{
	return bb;
}

std::string Mesh::getName()
{
	return name;
}
