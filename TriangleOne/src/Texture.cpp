#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include <glad/glad/glad.h> 


unsigned int TextureClass::LoadTextureFromFile(std::string name, std::string directory) {
	unsigned int textureID;
	int width;
	int height;
	int nrChannels;

	std::string totalPath = directory + "/" + name;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(totalPath.c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		std::cout << "fail to load the texture: " << name << std::endl;
		throw;
	}

	GLenum format = GL_RGB;
	if (nrChannels == 4)
		format = GL_RGBA;

	//Blabla OpenGl
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

unsigned int TextureClass::LoadCubeMapFromFile(std::vector<std::string> faces)
{
	unsigned int textureID;
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLenum format = GL_RGB;
			if (nrChannels == 4)
				format = GL_RGBA;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Cubemap failed to load at path: " << faces[i] << std::endl;

		stbi_image_free(data);
	}

	//Holy yapping
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

	return textureID;
}