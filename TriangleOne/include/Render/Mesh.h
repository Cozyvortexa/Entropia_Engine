#pragma once


#include <Render/SubMesh.h>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"

#include "ECS/Components/Component.h"

#include <variant>

using SubMeshStorage = std::variant<OpenGL_SubMesh>;

class Mesh {
public:
	Mesh() {};
	std::vector<SubMeshStorage> subMeshs;
	std::string directory;
	bool hasUV = true;
	bool hasTBN = true;
	bool hasNormalMap = true;

	//SubMesh factory
	void Create_SubMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int material_Handle) {
		Engine::Render::RendererAPI::API graphicAPI = Engine::Render::RendererAPI::GetAPI();

		switch (graphicAPI)
		{
		case Engine::Render::RendererAPI::API::None:
			std::cout << "RendererAPI is set on None" << std::endl;
			abort();
			break;
		case Engine::Render::RendererAPI::API::OpenGL:
			subMeshs.push_back(OpenGL_SubMesh(vertices, indices, material_Handle));
			break;
		case Engine::Render::RendererAPI::API::Vulkan:
			std::cout << "RendererAPI is set on Vulkan" << std::endl;
			abort();
			break;
		}
	}
};