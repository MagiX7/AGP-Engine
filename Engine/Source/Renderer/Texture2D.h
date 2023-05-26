#pragma once

#include <string>

class Texture2D
{
public:
	Texture2D(const std::string& filePath);
	virtual ~Texture2D();

	void Bind(uint32_t slot = 0);
	void Unbind();

	unsigned int GetId() { return id; }
	const std::string& GetName() { return name; }

private:
	unsigned int id;
	int channels;
	int internalFormat;
	int dataFormat;

	std::string path;
	std::string name;
	int width;
	int height;

};