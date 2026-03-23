#pragma once
#include "Render/Mesh.h"
#include <vector>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Render/Material.h"

class AssetStore {
public:

	Mesh& Get_Mesh(int index);
	std::pair<Mesh&, int> Get_Mesh(std::string path);

	std::pair<Material&, int> CreateMaterial(std::string name, const char* vertexPath, const char* fragmentPath);

	Material& Get_Material(std::string name);
	Material& Get_Material(int index);
private:
	std::vector<Mesh> meshs;
	std::unordered_map<std::string, int> pathToIndexMapMesh;

	std::vector<Material> materials;
	friend class LightSystem;  // WARNING, temp
	std::unordered_map<uint32_t, int> pathToIndexMapMaterial;

	int CheckExistingMat(std::string name);

	Mesh LoadMesh(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene, Mesh& currentMesh);
	SubMesh ProcessSub_Mesh(aiMesh* mesh, const aiScene* scene, Mesh& currentMesh);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene, Mesh& currentMesh);
};