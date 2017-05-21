#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include "ShaderProgram.h"
#include "Mesh.h"

enum LightType {
	point = 0,
	directional,
	spot
};

class Light
{
private:
	std::string name;
	LightType lightType;
	Mesh *mesh;

	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

	float specularStrength;

	//Attenuation values
	float attenuation_constant;
	float attenuation_linear;
	float attenuation_quadratic;
	
	glm::mat4 translateMatrix;
	glm::mat4 rotateMatrix;
	glm::mat4 modelMatrix;

	bool needsModelMatrixUpdate;

public:
	Light();
	Light(const std::string &name, LightType lightType, const glm::vec3 &position, glm::vec3 &ambientColor, const glm::vec3 &diffuseColor, const glm::vec3 &specularColor, Mesh *mesh);
	Light(const Light &light);
	~Light();

	void translate(const glm::vec3 &new_position);
	void rotate(GLfloat angle_deg, const glm::vec3 &axis);
	void showBB(bool visible);
	void draw();
	void drawBB();
	void uploadUniformsToShader(ShaderProgram *shader);

	std::string getName();
	bool isBBVisible();
	glm::vec3 &getAmbientColor();
	glm::vec3 &getDiffuseColor();
	glm::vec3 &getSpecularColor();
	glm::vec3 getPosition();
	glm::mat4 &getModelMatrix();

	Mesh *getMesh();
};

#endif 
