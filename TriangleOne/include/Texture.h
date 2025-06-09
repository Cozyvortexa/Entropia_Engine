#pragma once
#include <iostream>
#include <vector>
#include "string"

#include "stbImage/stb_image.h"


class TextureClass {
public:
	static unsigned int LoadTextureFromFile(std::string path, std::string directory);

	static unsigned int LoadCubeMapFromFile(std::vector<std::string> faces);
};