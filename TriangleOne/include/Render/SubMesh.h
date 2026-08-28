#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Render/RenderObject.h"

#include "Material.h"

struct Vertex {
	glm::vec3 Position = glm::vec3(0);
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent = glm::vec3(1,0,0);
};

struct Texture {
	Texture() = default;
	enum Type {
		None,
		Diffuse,
		Specular,
		Normal,
		Albedo,
		Ambient_Occlusion,
		Metalness,
		MetalicRoughness
	};
	Type textureType = None;
	unsigned int id = -1;
	uint64_t bindlessHandle = -1;
	std::string path = "";
};

class SubMesh {
public:
	virtual ~SubMesh() = default;
	// SubMesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::shared_ptr<Material> materialPtr;


	Engine::Render::VertexLayout layout;

protected: 
	SubMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Material> materialPtr);
};

class OpenGL_SubMesh : public SubMesh{
public:
	OpenGL_SubMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Material> materialPtr);
	//unsigned int VAO, VBO, EBO;
};