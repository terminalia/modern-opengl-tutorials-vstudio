#include "line.h"

Line::Line()
{

}

Line::Line(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 color)
{
	const float vertexData[] = {
		start.x, start.y, start.z,
		color.x, color.y, color.z,

		end.x, end.y, end.z,
		color.x, color.y, color.z
	};

	const unsigned int indices[] = {
		0, 1
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

	this->start = start;
	this->end = end;
	this->color = color;
}

Line::Line(const Line &line)
{
	vbo_vertexdata = line.vbo_vertexdata;
	vbo_indices = line.vbo_indices;
	vao = line.vao;
	start = line.start;
	end = line.end;
	color = line.color;
}

Line::~Line()
{

}

void Line::cleanGLBuffers()
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

void Line::draw()
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
}

void Line::update(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
{
	this->start = start;
	this->end = end;
	this->color = color;

	const float vertexData[] = {
		this->start.x, this->start.y, this->start.z,
		this->color.x, this->color.y, this->color.z,

		this->end.x, this->end.y, this->end.z,
		this->color.x, this->color.y, this->color.z
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexdata);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
}
