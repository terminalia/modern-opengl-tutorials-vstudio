#ifndef GRID_H
#define GRID_H

#include <GL/glew.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include "line.h"

class Grid
{
private:
	std::vector<Line> lines;

public:
	Grid();
	Grid(GLuint numOfLines, GLfloat spacing);
	Grid(const Grid &grid);
	~Grid();

	void cleanGLBuffers();
	void draw();
};

#endif // GRID_H
