#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include "messages.h"
#include <vector>

std::map<std::string, std::map<int,int>> Texture::textures;

Texture::Texture(const char * path)
{
	id = 0;
	int width, height, nrChannels;//R"(D:\Projects\C++\KatEngine\Pukman\winter.jpg)"
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (!data)
		fatal_error("Couldn't load texture image");
	this->width = width;
	this->height = height;
	glGenTextures(1, &id);
	//glActiveTexture(GL_TEXTURE0);//????
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(int width, int height)
{
	this->width = width;
	this->height = height;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);
	memset(data, 0, sizeof(unsigned char) * width * height * 3);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);

	free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


Texture::~Texture()
{
}

void Texture::bindTexture(int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::bindImage(int unit, int access)
{
	/*
	 * GLuint unit,
 	GLuint texture,
 	GLint level,
 	GLboolean layered,
 	GLint layer,
 	GLenum access,
 	GLenum format);
	 */
	glBindImageTexture(unit, id, 0, GL_FALSE, 0, access, GL_RGBA32F);
}

void Texture::generateMipmap()
{
	bindTexture(0);
	glGenerateMipmap(GL_TEXTURE_2D);

}

void Texture::clear()
{
	glBindTexture(GL_TEXTURE_2D, id);

	unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);
	memset(data, 0, sizeof(unsigned char) * width * height * 3);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	free(data);
}

Texture Texture::FromGenFunction(int width, int height, function<void(int*, int, int)> genFunc)
{
	Texture tex(width, height);
	tex.bindTexture(0);

	int* data = new int[width*height*3];

	genFunc(data, width, height);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	delete[] data;
	
	return tex;
}
