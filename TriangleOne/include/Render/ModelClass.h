#pragma once


#include <Render/MeshClass.h>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"

#include "ECS/Components/Component.h"

class Model {
public:
	Model() {};
	//Model(std::string path)
	//{
	//	LoadModel(path);
	//}
	void Draw(Shader* shader) {
		for (Mesh& mesh : meshes)
			mesh.Draw(shader);
	}
	void DrawWithoutTexture(Shader* shader) {
		for (Mesh& mesh : meshes)
			mesh.DrawWithoutTexture(shader);
	}
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	bool hasUV = true;
	bool hasTBN = true;

};