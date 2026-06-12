#pragma once
#include "Render/Mesh.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <cstdint>
#include <functional>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Render/Material.h"

#include "Audio/AudioStruct.h"
#include "ECS/Resources/Resource.h"

class AssetStore {
public:
	AssetStore() { meshs.reserve(2000); textures.reserve(2000); }
	Mesh& Get_Mesh(int index);
	std::pair<Mesh&, int> Get_Mesh(std::string path);

	std::pair<Material&, int> CreateMaterial(std::string name, const char* vertexPath, const char* fragmentPath);

	Material& Get_Material(std::string name);
	Material* Get_Material(unsigned int index);

	Texture* Get_Texture(unsigned int index);


	Engine::Audio::Audio* Load_Sound(ma_engine& audioEngine, const std::string& name, const char* path, Engine::Audio::SoundFlags flags);

private:
	//Assets
	std::vector<Mesh> meshs;
	std::vector<Texture> textures;
	std::vector<Material> materials;

	//Map
	std::unordered_map<std::string, int> pathToIndexMapMesh;
	std::unordered_map<std::string, unsigned int> pathToIndexMap_Texture;
	std::unordered_map<size_t, unsigned int> keyTo_MaterialHandle;
	std::unordered_map<uint32_t, int> pathToIndexMapMaterial;

	//Misc
	int CheckExistingMat(std::string name);


	//Mesh creation
	Mesh LoadMesh(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene, Mesh& currentMesh, std::string path);
	void ProcessSub_Mesh(aiMesh* mesh, const aiScene* scene, Mesh& currentMesh, std::string path);
	unsigned int LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene, Mesh& currentMesh, std::string path);

	//friend
	friend class LightSystem;  // WARNING, temp
};