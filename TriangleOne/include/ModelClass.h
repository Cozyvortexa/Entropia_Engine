#pragma once

#include <MeshClass.h>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Model
{
public:
	Model(char* path)
	{
		LoadModel(path);
	}
	void Draw(Shader& shader);
private:
	// model data
	std::vector<Mesh> meshes;
	std::string directory;
	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

};