#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include "Mesh.h"
#include "Texture.h"

class Model
{
private:
	std::string name;
	Mesh *mesh;
	Texture *texture;

	glm::mat4 translateMatrix;
	glm::mat4 rotateMatrix;
	glm::mat4 scaleMatrix;
	glm::mat4 modelMatrix;
	glm::mat4 normalMatrix;

	bool needsModelMatrixUpdate;

public:
	Model();
	Model(const std::string &name, Mesh *mesh, Texture *texture);
	Model(const Model &model);
	~Model();

	void translate(const glm::vec3 &new_position);
	void rotate(GLfloat angle_deg, const glm::vec3 &axis);
	void scale(const glm::vec3 &scaling);

	std::string getName();
	glm::mat4 &getModelMatrix();
	glm::mat4 &getNormalMatrix();
	glm::vec3 getPosition();

	Mesh *getMesh();
	Texture *getTexture();
};

#endif