#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include <glad/glad/glad.h> 


TextureClass::TextureClass(const char* texturePath) {

	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (!data) {
		std::cout << "fail to load the texture: " << texturePath << std::endl;
		throw;
	}

	GLenum format = GL_RGB;
	if (nrChannels == 4) // Bug non citer dans le pdf
		format = GL_RGBA;

	//Blabla OpenGl
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

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
}

unsigned int TextureClass::LoadTextureFromFile(std::string name, std::string directory) {
	unsigned int textureID;  // Jreglerais sa plus tard 
	int _width;
	int _height;
	int _nrChannels;

	std::string totalPath = directory + "/" + name;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(totalPath.c_str(), &_width, &_height, &_nrChannels, 0);
	if (!data) {
		std::cout << "fail to load the texture: " << name << std::endl;
		throw;
	}

	GLenum format = GL_RGB;
	if (_nrChannels == 4) // Bug non citer dans le pdf
		format = GL_RGBA;

	//Blabla OpenGl
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//

	glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}