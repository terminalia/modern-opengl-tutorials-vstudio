#include "orbitcamera.h"

OrbitCamera::OrbitCamera()
{

}

OrbitCamera::OrbitCamera(GLfloat fov, GLfloat radius, GLfloat start_theta, GLfloat start_phi)
{
	this->fov = fov;
	this->radius = radius;
	theta = 0.0f;
	phi = 0.0f;
	rotate(glm::radians(start_theta), glm::radians(start_phi));
}

OrbitCamera::OrbitCamera(const OrbitCamera &camera)
{
	this->fov = camera.fov;
	this->radius = camera.radius;
	this->theta = camera.theta;
	this->phi = camera.phi;
	this->position = camera.position;
	this->target = camera.target;
	this->up = camera.up;
	this->right = camera.right;
	this->ratio = camera.ratio;
}

OrbitCamera::~OrbitCamera()
{

}

void OrbitCamera::rotate(GLfloat x, GLfloat y)
{
	phi += y;
	//If camera is upside down, invert x axis rotation
	if (up.y < 0.0f) {
		theta -= x;
	}
	else {
		theta += x;
	}

	computePosition();
}

void OrbitCamera::pan(float offsetX, float offsetY)
{
	target = target + (right * offsetX) + (up * offsetY);
	computePosition();
}

void OrbitCamera::zoom(float distance)
{

	radius -= distance;
	if (radius < 0.1f)
		radius = 0.1f;
	/*
	fov -= distance;

	if (fov <= 0.0f) {
	fov = 0.0f;
	}
	*/

	computePosition();
}

void OrbitCamera::computePosition()
{
	GLfloat camPosX = cos(theta) * cos(phi) * radius;
	GLfloat camPosY = sin(phi) * radius;
	GLfloat camPosZ = sin(theta) * cos(phi) * radius;

	position = glm::vec3(camPosX, camPosY, camPosZ);
	position = target + position;
	right = glm::vec3(cos(theta + glm::radians(90.0f)), 0, sin(theta + glm::radians(90.0f)));
	up = glm::cross(right, position);
}

void OrbitCamera::updateViewRatio(GLfloat ratio)
{
	this->ratio = ratio;
}

glm::mat4 OrbitCamera::getViewMatrix() const
{
	return glm::lookAt(position, target, up);
}

glm::mat4 OrbitCamera::getProjectionMatrix() const
{
	return glm::perspective(glm::radians(fov), ratio, 0.1f, 1000.0f);
}

glm::vec3 OrbitCamera::getPosition() const
{
	return position;
}
