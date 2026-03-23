#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent = glm::vec3(1,0,0);
};

struct Texture {
	enum Type {
		None,
		Diffuse,
		Specular,
		Normal
	};
	Type textureType = None; // Valeur par default
	unsigned int id;
	std::string path;
};

class Mesh {
public:
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	void Draw(Shader* shader);
	void DrawWithoutTexture(Shader* shader);
private:
	// render data
	unsigned int VAO, VBO, EBO;
	void SetupMesh();
};