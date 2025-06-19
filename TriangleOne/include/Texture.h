#pragma once
#include <iostream>
#include <vector>
#include "string"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stbImage/stb_image.h"


class TextureClass {
public:
	static unsigned int LoadTextureFromFile(std::string path, std::string directory);

	static unsigned int LoadEmbeddedTexture(aiTexture* tex);

	static unsigned int LoadCubeMapFromFile(std::vector<std::string> faces);
};