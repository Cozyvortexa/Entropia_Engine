#pragma once
#include <iostream>


#include "stbImage/stb_image.h"


class Texture {
public:
	Texture(const char* texturePath);

	unsigned int getTexture() { return texture; }
private:
	unsigned int texture;
	int width;
	int height;
	int nrChannels;
};