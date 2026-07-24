#pragma once
#include <Render/SubMesh.h>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"

#include <variant>

using SubMeshStorage = std::variant<OpenGL_SubMesh>;

struct InstanceGroup { 
	InstanceGroup() = default;

	unsigned int subMesh = 0;
	std::vector<glm::mat4> instancedMatrix;
};

class Mesh {
public:
	Mesh() {};
	std::vector<SubMeshStorage> subMeshs;
	std::string directory = "None";
	bool hasUV = true;
	bool hasTBN = true;
	bool hasNormalMap = true;
	bool isValid = false;

	std::vector<InstanceGroup> instancesGroup;

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

	std::pair<std::vector<glm::vec3>, std::vector<unsigned int>> Get_VerticesAndIndices() {
		std::vector<glm::vec3> vertices;
		std::vector<unsigned int> indices;

		for (const SubMeshStorage& current_SubMesh : subMeshs) {
			std::visit([&vertices, &indices](auto& mesh) {
				const SubMesh& subMesh = static_cast<const SubMesh&>(mesh);
				unsigned int baseIndex = static_cast<unsigned int>(vertices.size());

				for (const auto& vertex : subMesh.vertices)
					vertices.push_back(vertex.Position);

				for (unsigned int idx : subMesh.indices)
					indices.push_back(baseIndex + idx);

				}, current_SubMesh);
		}
		return std::make_pair(vertices, indices);
	}
};