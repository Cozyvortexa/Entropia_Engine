#pragma once


#include <Systemes/RenderModule/MeshClass.h>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"

#include "ECS/Component.h"

class Model : Component {
public:
	Model() {};
	//Model(std::string path)
	//{
	//	LoadModel(path);
	//}
	//void Draw(std::shared_ptr<Shader> shader);
	void DrawWithoutTexture(std::shared_ptr<Shader> shader) {
		for (Mesh& mesh : meshes)
			mesh.DrawWithoutTexture(shader);
	}
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

private:
	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene);

};

//void Model::Draw(std::shared_ptr<Shader> shader) {
//	for (Mesh& mesh : meshes)
//		mesh.Draw(shader);
//}