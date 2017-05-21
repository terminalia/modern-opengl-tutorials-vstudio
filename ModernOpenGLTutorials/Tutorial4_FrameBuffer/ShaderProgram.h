#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <GL/glew.h>

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


	void update();
	void cleanUp();
	GLuint getProgram() const;
	std::string getName();
};

#endif // SHADERPROGRAM_H
