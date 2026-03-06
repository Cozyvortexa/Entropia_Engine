#pragma once
#include "Systemes/RenderModule/ModelClass.h"
#include <vector>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "ECS/Material.h"

class ModelStore {
public:

	Model& Get_Model(int index);
	std::pair<Model&, int> Get_Model(std::string path);

	std::pair<Material&, int> CreateMaterial(std::string name, const char* vertexPath, const char* fragmentPath);

	Material& Get_Material(std::string name);
	Material& Get_Material(int index);
private:
	std::vector<Model> models;
	std::unordered_map<std::string, int> pathToIndexMapModel;

	std::vector<Material> materials;
	std::unordered_map<uint32_t, int> pathToIndexMapMaterial;

	int CheckExistingMat(std::string name);

	Model LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene, Model& currentModel);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, Model& currentModel);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene, Model& currentModel);
};