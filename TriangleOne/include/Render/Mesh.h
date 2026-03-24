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
	std::vector<SubMesh> subMeshs;
	std::string directory;
	bool hasUV = true;
	bool hasTBN = true;
	bool hasNormalMap = true;
};