#include "Model.h"



Model::Model()
{
}

Model::Model(const std::string & name, Mesh * mesh, Texture * texture)
{
	this->name = name;
	this->mesh = mesh;
	this->texture = texture;

	modelMatrix = glm::mat4(1.0f);
	translateMatrix = glm::mat4(1.0f);
	rotateMatrix = glm::mat4(1.0f);
	scaleMatrix = glm::mat4(1.0f);
	normalMatrix = glm::transpose(glm::inverse(modelMatrix));
}

Model::Model(const Model &model)
{
	name = model.name;
	mesh = model.mesh;
	texture = model.texture;

	modelMatrix = model.modelMatrix;
	translateMatrix = model.translateMatrix;
	rotateMatrix = model.rotateMatrix;
	scaleMatrix = model.scaleMatrix;
	normalMatrix = model.normalMatrix;
	needsModelMatrixUpdate = model.needsModelMatrixUpdate;
}

Model::~Model()
{

}

void Model::translate(const glm::vec3 & new_position)
{
	needsModelMatrixUpdate = true;
	translateMatrix = glm::translate(glm::mat4(1.0f), new_position);
}

void Model::rotate(GLfloat angle_deg, const glm::vec3 & axis)
{
	needsModelMatrixUpdate = true;
	rotateMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle_deg), axis);
}

void Model::scale(const glm::vec3 & scaling)
{
	needsModelMatrixUpdate = true;
	scaleMatrix = glm::scale(glm::mat4(1.0f), scaling);
}

std::string Model::getName()
{
	return name;
}

glm::mat4 & Model::getModelMatrix()
{
	if (needsModelMatrixUpdate)
	{
		modelMatrix = translateMatrix * rotateMatrix * scaleMatrix;
		needsModelMatrixUpdate = false;
	}

	return modelMatrix;
}

glm::mat4 & Model::getNormalMatrix()
{
	if (needsModelMatrixUpdate)
	{
		normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	}
	return normalMatrix;
}

glm::vec3 Model::getPosition()
{
	return glm::vec3(modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z);
}

Mesh * Model::getMesh()
{
	return mesh;
}

Texture * Model::getTexture()
{
	return texture;
}


