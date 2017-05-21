#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <GL/glew.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

class ShaderProgram
{
private:
	std::string name;
	std::string vertexShaderFile;
	std::string fragmentShaderFile;
	GLuint programId;

private:
	void init();
	void link(GLuint, GLuint);

	void checkCompilationErrors(GLuint, GLenum type);
	void checkLinkErros();
	GLuint addShader(const std::string &, GLenum type);
	std::string loadSourceFromFile(const std::string &);

public:
	ShaderProgram();
	ShaderProgram(const std::string &name, const std::string &vertexShaderFile, const std::string &fragmentShaderFile);
	ShaderProgram(const ShaderProgram &shaderProgram);
	~ShaderProgram();

	void uploadBool(const std::string &uniformLoc, bool value);
	void uploadInt(const std::string &uniformLoc, int value);
	void uploadFloat(const std::string &uniformLoc, float value);
	void uploadVec2(const std::string &uniformLoc, const glm::vec2 &value);
	void uploadVec3(const std::string &uniformLoc, const glm::vec3 &value);
	void uploadVec4(const std::string &uniformLoc, const glm::vec4 &value);
	void uploadMat3(const std::string &uniformLoc, const glm::mat3 &value);
	void uploadMat4(const std::string &uniformLoc, const glm::mat4 &value);

	void update();
	void bind();
	void unbind();
	void cleanUp();
	GLuint getProgram() const;
	std::string getName();
};

#endif // SHADERPROGRAM_H
