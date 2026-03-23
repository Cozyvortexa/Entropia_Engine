#pragma once


#include <Render/SubMesh.h>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"

#include "ECS/Components/Component.h"

class Mesh {
public:
	Mesh() {};
	//Mesh(std::string path)
	//{
	//	LoadMesh(path);
	//}
	void Draw(Shader* shader) {
		for (SubMesh& sub_Mesh : subMeshs)
			sub_Mesh.Draw(shader);
	}
	void DrawWithoutTexture(Shader* shader) {
		for (SubMesh& sub_Mesh : subMeshs)
			sub_Mesh.DrawWithoutTexture(shader);
	}
	std::vector<SubMesh> subMeshs;
	std::string directory;
	std::vector<Texture> textures_loaded;
	bool hasUV = true;
	bool hasTBN = true;

};