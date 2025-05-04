#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include <glad/glad/glad.h> 


Texture::Texture(const char* texturePath) {

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