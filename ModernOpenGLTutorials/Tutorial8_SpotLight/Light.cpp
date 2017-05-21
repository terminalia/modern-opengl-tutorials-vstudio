#include "Light.h"



Light::Light()
{
}

Light::Light(const std::string & name, LightType lightType, const glm::vec3 &position, glm::vec3 & ambientColor, const glm::vec3 & diffuseColor, const glm::vec3 & specularColor, float constant, float linear, float quadratic, Mesh * mesh)
{
	this->name = name;
	this->lightType = lightType;
	this->ambientColor = ambientColor;
	this->diffuseColor = diffuseColor;
	this->specularColor = specularColor;
	this->mesh = mesh;

	needsModelMatrixUpdate = false;

	modelMatrix = glm::mat4(1.0f);
	translateMatrix = glm::mat4(1.0f);
	rotateMatrix = glm::mat4(1.0f);

	attenuation_constant = constant;
	attenuation_linear = linear;
	attenuation_quadratic = quadratic;
	specularStrength = 64;

	translate(position);
}

Light::Light(const std::string & name, LightType lightType, const glm::vec3 & direction, glm::vec3 & ambientColor, const glm::vec3 & diffuseColor, const glm::vec3 & specularColor, Mesh * mesh)
{
	this->name = name;
	this->lightType = lightType;
	this->ambientColor = ambientColor;
	this->diffuseColor = diffuseColor;
	this->specularColor = specularColor;
	this->mesh = mesh;
	this->direction = direction;

	needsModelMatrixUpdate = false;

	modelMatrix = glm::mat4(1.0f);
	translateMatrix = glm::mat4(1.0f);
	rotateMatrix = glm::mat4(1.0f);
	specularStrength = 64;

	//Default position
	translate(glm::vec3(0, 10, 0));
}

Light::Light(const std::string & name, LightType lightType, const glm::vec3 &position, const glm::vec3 & direction, glm::vec3 & ambientColor, const glm::vec3 & diffuseColor, const glm::vec3 & specularColor, float constant, float linear, float quadratic, float cutOff, float outerCutOff, Mesh * mesh)
{
	this->name = name;
	this->lightType = lightType;
	this->ambientColor = ambientColor;
	this->diffuseColor = diffuseColor;
	this->specularColor = specularColor;
	this->mesh = mesh;
	this->direction = direction;

	needsModelMatrixUpdate = false;

	modelMatrix = glm::mat4(1.0f);
	translateMatrix = glm::mat4(1.0f);
	rotateMatrix = glm::mat4(1.0f);

	attenuation_constant = constant;
	attenuation_linear = linear;
	attenuation_quadratic = quadratic;
	specularStrength = 64;

	this->cutOff = cutOff;
	this->outerCutOff = outerCutOff;
	
	translate(position);
}

Light::Light(const Light &light)
{
	name = light.name;
	lightType = light.lightType;
	ambientColor = light.ambientColor;
	diffuseColor = light.diffuseColor;
	specularColor = light.specularColor;
	specularStrength = light.specularStrength;
	attenuation_constant = light.attenuation_constant;
	attenuation_linear = light.attenuation_linear;
	attenuation_quadratic = light.attenuation_quadratic;
	mesh = light.mesh;

	modelMatrix = light.modelMatrix;
	translateMatrix = light.translateMatrix;
	rotateMatrix = light.rotateMatrix;
	needsModelMatrixUpdate = light.needsModelMatrixUpdate;
	cutOff = light.cutOff;
	outerCutOff = light.outerCutOff;
}


Light::~Light()
{
}

void Light::translate(const glm::vec3 & new_position)
{
	needsModelMatrixUpdate = true;
	translateMatrix = glm::translate(glm::mat4(1.0f), new_position);
}

void Light::rotate(GLfloat angle_deg, const glm::vec3 & axis)
{
	needsModelMatrixUpdate = true;
	rotateMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle_deg), axis);
}

void Light::showBB(bool visible)
{
	mesh->setBBVisible(visible);
}

void Light::draw()
{
	mesh->draw();
}

void Light::drawBB()
{
	mesh->drawBB();
}

void Light::uploadUniformsToShader(ShaderProgram *shader)
{
	if (lightType == point)
	{
		shader->uploadVec3("pointLight.position", getPosition());
		shader->uploadVec3("pointLight.ambient", ambientColor);
		shader->uploadVec3("pointLight.diffuse", diffuseColor);
		shader->uploadVec3("pointLight.specular", specularColor);
		shader->uploadFloat("pointLight.specularStrength", specularStrength);
		shader->uploadFloat("pointLight.constant", attenuation_constant);
		shader->uploadFloat("pointLight.linear", attenuation_linear);
		shader->uploadFloat("pointLight.quadratic", attenuation_quadratic);
	}

	if (lightType == directional)
	{
		shader->uploadVec3("dirLight.direction", direction);
		shader->uploadVec3("dirLight.ambient", ambientColor);
		shader->uploadVec3("dirLight.diffuse", diffuseColor);
		shader->uploadVec3("dirLight.specular", specularColor);
		shader->uploadFloat("dirLight.specularStrength", specularStrength);
	}

	if (lightType == spot)
	{
		shader->uploadVec3("spotLight.position", getPosition());
		shader->uploadVec3("spotLight.direction", direction);
		shader->uploadVec3("spotLight.ambient", ambientColor);  
		shader->uploadVec3("spotLight.diffuse", diffuseColor);
		shader->uploadVec3("spotLight.specular", specularColor);
		shader->uploadFloat("spotLight.specularStrength", specularStrength);
		shader->uploadFloat("spotLight.constant", attenuation_constant);
		shader->uploadFloat("spotLight.linear", attenuation_linear);
		shader->uploadFloat("spotLight.quadratic", attenuation_quadratic);
		shader->uploadFloat("spotLight.cutOff", glm::cos(glm::radians(cutOff)));
		shader->uploadFloat("spotLight.outerCutOff", glm::cos(glm::radians(outerCutOff)));
	}
}

std::string Light::getName()
{
	return name;
}

bool Light::isBBVisible()
{
	return mesh->isBBVisible();
}

glm::vec3 & Light::getAmbientColor()
{
	return ambientColor;
}

glm::vec3 & Light::getDiffuseColor()
{
	return diffuseColor;
}

glm::vec3 & Light::getSpecularColor()
{
	return specularColor;
}

glm::mat4 & Light::getModelMatrix()
{
	if (needsModelMatrixUpdate)
	{
		modelMatrix = translateMatrix * rotateMatrix;
		needsModelMatrixUpdate = false;
	}

	return modelMatrix;
}

Mesh * Light::getMesh()
{
	return mesh;
}

glm::vec3 Light::getPosition()
{
	return glm::vec3(modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z);
}