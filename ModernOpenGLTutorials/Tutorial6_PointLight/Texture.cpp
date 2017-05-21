#include "texture.h"

Texture::Texture()
{

}

Texture::Texture(const std::string &name, GLenum textureTarget, int width, int height)
{
	this->name = name;
	this->textureTarget = textureTarget;
	this->width = width;
	this->height = height;

	glGenTextures(1, &textureId);
	glBindTexture(textureTarget, textureId);
	//Create an empty texture (0 at the end)
	glTexImage2D(textureTarget, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(textureTarget, 0);
}

Texture::Texture(const std::string &name, GLenum textureTarget, const std::string &filename)
{
	this->name = name;
	this->textureTarget = textureTarget;

	unsigned char *image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	if (!image)
	{
		std::string error = "Failed loading image file " + filename + "!";
		throw std::runtime_error(error);
	}

	glGenTextures(1, &textureId);
	glBindTexture(textureTarget, textureId);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(textureTarget, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(textureTarget);
	SOIL_free_image_data(image);
	glBindTexture(textureTarget, 0);
}

Texture::Texture(const Texture &texture)
{
	this->name = texture.name;
	this->textureId = texture.textureId;
	this->textureTarget = texture.textureTarget;
	this->width = texture.width;
	this->height = texture.height;
}

Texture::~Texture()
{

}

void Texture::cleanGLBuffers()
{
	glDeleteTextures(1, &textureId);
}

void Texture::bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(textureTarget, textureId);

}

void Texture::unbind()
{
	glActiveTexture(0);
	glBindTexture(textureTarget, 0);
}

std::string Texture::getName()
{
	return name;
}

GLuint &Texture::getTextureId()
{
	return textureId;
}
