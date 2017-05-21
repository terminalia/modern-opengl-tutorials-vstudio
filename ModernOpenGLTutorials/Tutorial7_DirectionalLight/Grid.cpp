#include "grid.h"

Grid::Grid()
{

}

Grid::Grid(GLuint numOfLines, GLfloat spacing)
{
	GLint halfNumOfLines = numOfLines / 2;
	glm::vec3 color(1, 1, 1);

	for (unsigned int row = 0; row <= numOfLines; row++)
	{
		glm::vec3 start(-halfNumOfLines, 0.0f, halfNumOfLines - spacing * row);
		glm::vec3 end(halfNumOfLines, 0.0f, halfNumOfLines - spacing * row);

		lines.push_back(Line(start, end, color));
	}

	for (unsigned int col = 0; col <= numOfLines; col++)
	{
		glm::vec3 start(-halfNumOfLines + spacing * col, 0.0f, -halfNumOfLines);
		glm::vec3 end(-halfNumOfLines + spacing * col, 0.0f, halfNumOfLines);
		lines.push_back(Line(start, end, color));;
	}
}

Grid::Grid(const Grid &grid)
{
	this->lines = grid.lines;
}

Grid::~Grid()
{
}


void Grid::cleanGLBuffers()
{
	for (unsigned int i = 0; i<lines.size(); i++)
	{
		lines.at(i).cleanGLBuffers();
	}
}

void Grid::draw()
{
	for (unsigned int i = 0; i<lines.size(); i++)
	{
		lines.at(i).draw();
	}
}
