#pragma once
#include <iostream>


#include "stbImage/stb_image.h"


class TextureClass {
public:
	TextureClass(const char* texturePath);

	unsigned int getTexture() { return texture; }

	static unsigned int LoadTextureFromFile(std::string path, std::string directory);
private:
	unsigned int texture;
	int width;
	int height;
	int nrChannels;
};