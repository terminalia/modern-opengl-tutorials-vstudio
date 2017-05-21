#ifndef ORBITCAMERA_H
#define ORBITCAMERA_H

#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

class OrbitCamera
{
private:
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up;
	glm::vec3 right;

	GLfloat radius;
	GLfloat ratio;
	GLfloat fov;
	GLfloat theta;
	GLfloat phi;

	void computePosition();

public:
	OrbitCamera();
	OrbitCamera(GLfloat fov, GLfloat radius, GLfloat start_theta, GLfloat start_phi);
	OrbitCamera(const OrbitCamera &camera);
	~OrbitCamera();

	void rotate(GLfloat x, GLfloat y);
	void updateViewRatio(GLfloat ratio);
	void pan(float offsetX, float offsetY);
	void zoom(float distance);

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
	glm::vec3 getPosition() const;
};

#endif // ORBITCAMERA_H
