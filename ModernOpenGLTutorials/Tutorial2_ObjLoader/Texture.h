#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <exception>
#include <GL/glew.h>
#include <SOIL.h>

class Texture
{
private:
	GLuint textureId;
	GLenum textureTarget;
	std::string name;
	int width;
	int height;

public:
	Texture();
	Texture(const std::string &name, GLenum textureTarget, const std::string &filename);
	Texture(const Texture &texture);
	~Texture();

	void cleanGLBuffers();
	void bind(GLenum textureUnit);
	void unbind();
	std::string getName();
};

#endif // TEXTURE_H
