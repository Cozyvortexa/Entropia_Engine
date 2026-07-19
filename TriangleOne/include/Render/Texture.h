#pragma once
#include <iostream>
#include <vector>
#include "string"

#include <assimp/scene.h>

#include "stbImage/stb_image.h"
#include "PathFolder.h"


class TextureClass {
public:
	static unsigned int Load_OpenGL_Texture(unsigned char* data, int width, int height, int nrChannels);

	static unsigned int LoadEmbeddedTexture(const aiTexture* tex);

	static unsigned int LoadCubeMapFromFile(std::vector<std::string> faces);

	static unsigned int LoadEquirectangularTex(std::string path);
};