#pragma once
#include <string>
#include <map>
#include <glad/glad.h>
#include <functional>
using namespace std;

class Texture
{
public:
	static Texture FromGenFunction(int width, int height, function<void(int*, int, int)> genFunc);
	static map<string, map<int, int>> textures;

	static const int READ_ONLY = GL_READ_ONLY;
	static const int WRITE_ONLY = GL_WRITE_ONLY;
	static const int READ_WRITE = GL_READ_WRITE;

	unsigned int id;
	std::string type;
	std::string path;
	int width, height;
	
	Texture(const char * path);
	Texture(int width, int height);
	~Texture();

	void bindTexture(int unit);
	void bindImage(int unit, int access = GL_WRITE_ONLY);
	void generateMipmap();
	void clear();


};

