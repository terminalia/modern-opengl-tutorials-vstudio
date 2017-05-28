#include "boundingbox.h"

BoundingBox::BoundingBox()
{

}

BoundingBox::BoundingBox(const glm::vec3 &min_vertex, const glm::vec3 &max_vertex, const glm::vec3 &color)
{
	this->color = color;
	this->min_vertex = min_vertex;
	this->max_vertex = max_vertex;

	GLfloat vertexData[] = {
		min_vertex.x, min_vertex.y, min_vertex.z,   //V0
		color.x, color.y, color.z,
		max_vertex.x, min_vertex.y, min_vertex.z,   //V1
		color.x, color.y, color.z,
		max_vertex.x, min_vertex.y, max_vertex.z,   //V2
		color.x, color.y, color.z,
		min_vertex.x, min_vertex.y, max_vertex.z,   //V3
		color.x, color.y, color.z,
		min_vertex.x, max_vertex.y, min_vertex.z,   //V4
		color.x, color.y, color.z,
		max_vertex.x, max_vertex.y, min_vertex.z,   //V5
		color.x, color.y, color.z,
		max_vertex.x, max_vertex.y, max_vertex.z,   //V6
		color.x, color.y, color.z,
		min_vertex.x, max_vertex.y, max_vertex.z,   //V7
		color.x, color.y, color.z
	};

	/*
	Triangles BBox
	GLushort indices[] = {
	0, 1, 2, 0, 2, 3,   //Bottom
	2, 6, 7, 2, 7, 3,   //Front
	4, 5, 1, 4, 1, 0,   //Back
	6, 5, 4, 6, 4, 7,   //Top
	1, 5, 6, 1, 6, 2,  //Right
	0, 7, 4, 0, 3, 7   //Left
	};
	*/

	GLushort indices[] = {
		2, 6, 7, 3,
		0, 4, 5, 1,
		7, 4, 6, 5,
		3, 0, 2, 1
	};

	glGenBuffers(1, &vbo_vertexdata);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexdata);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);


	glGenBuffers(1, &vbo_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexdata);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
}

BoundingBox::BoundingBox(const BoundingBox &boundingBox)
{
	color = boundingBox.color;
	min_vertex = boundingBox.min_vertex;
	max_vertex = boundingBox.max_vertex;
	vbo_vertexdata = boundingBox.vbo_vertexdata;
	vbo_indices = boundingBox.vbo_indices;
	vao = boundingBox.vao;
}

BoundingBox::~BoundingBox()
{

}

void BoundingBox::draw()
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	//Front
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	//Back
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
}

void BoundingBox::cleanGLBuffers()
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo_indices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo_vertexdata);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);
}

glm::vec3 BoundingBox::getMinVertex()
{
	return min_vertex;
}

glm::vec3 BoundingBox::getMaxVertex()
{
	return max_vertex;
}
