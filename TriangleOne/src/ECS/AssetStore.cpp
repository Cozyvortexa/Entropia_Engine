#include "ECS/AssetStore.h"

Mesh AssetStore::LoadMesh(std::string path) {
	Mesh mesh = Mesh();
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		abort();  //Add here the default object to report the fail
		//return;  
	}
	//std::cout << "Embedded textures: " << scene->mNumTextures << std::endl;
	//std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;
	//std::cout << "Number of materials: " << scene->mNumMaterials << std::endl;
	mesh.directory = path.substr(0, path.find_last_of("/"));
	ProcessNode(scene->mRootNode, scene, mesh);
	return mesh;
}

void AssetStore::ProcessNode(aiNode* node, const aiScene* scene, Mesh& currentMesh)
{
	// process all the node’s meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* subMesh = scene->mMeshes[node->mMeshes[i]];
		if (!subMesh->HasTextureCoords(0)) currentMesh.hasUV = false;
		currentMesh.subMeshs.push_back(ProcessSub_Mesh(subMesh, scene, currentMesh));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, currentMesh);
	}
}

SubMesh AssetStore::ProcessSub_Mesh(aiMesh* sub_Mesh, const aiScene* scene, Mesh& currentMesh)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < sub_Mesh->mNumVertices; i++)
	{
		Vertex vertex;
		//Position
		glm::vec3 vector;
		vector.x = sub_Mesh->mVertices[i].x;
		vector.y = sub_Mesh->mVertices[i].y;
		vector.z = sub_Mesh->mVertices[i].z;
		vertex.Position = vector;

		//PosTex
		if (sub_Mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = sub_Mesh->mTextureCoords[0][i].x;
			vec.y = sub_Mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		//Tangent
		if (sub_Mesh->HasTangentsAndBitangents()) {
			glm::vec3 tangentVec;
			tangentVec.x = sub_Mesh->mTangents[i].x;
			tangentVec.y = sub_Mesh->mTangents[i].y;
			tangentVec.z = sub_Mesh->mTangents[i].z;
			vertex.Tangent = tangentVec;
		}
		else
			currentMesh.hasTBN = false;

		vertices.push_back(vertex);
	}
	//Indices
	for (unsigned int i = 0; i < sub_Mesh->mNumFaces; i++)
	{
		aiFace face = sub_Mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (sub_Mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[sub_Mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, scene, currentMesh);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());


		std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, scene, currentMesh);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());


		std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, scene, currentMesh);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	return SubMesh(vertices, indices, textures);
}

std::vector<Texture> AssetStore::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene, Mesh& currentMesh)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		for (unsigned int j = 0; j < currentMesh.textures_loaded.size(); j++)
		{
			//std::string fullPath = directory + "/" + std::string(str.C_Str());
			if (std::strcmp(currentMesh.textures_loaded[j].path.data(), str.C_Str()) == 0)  // Verifie si la texture a deja etait charger
			{
				textures.push_back(currentMesh.textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture texture;

			if (str.C_Str()[0] == '*') {  // texture embarquer detecter
				int texIndex = atoi(str.C_Str() + 1);
				aiTexture* EmbeddedTex = scene->mTextures[texIndex];
				texture.id = TextureClass::LoadEmbeddedTexture(EmbeddedTex);
			}
			else
				texture.id = TextureClass::LoadTextureFromFile(str.C_Str(), currentMesh.directory);

			texture.path = str.C_Str();

			switch (type) {  // la texture est charger meme si elle ne serra pas utiliser
			case aiTextureType_DIFFUSE:
				texture.textureType = Texture::Diffuse;
				break;
			case aiTextureType_SPECULAR:
				texture.textureType = Texture::Specular;
				break;
			case aiTextureType_NORMALS:
				texture.textureType = Texture::Normal;
				break;
			}

			textures.push_back(texture);
			currentMesh.textures_loaded.push_back(texture);

		}

	}
	return textures;
}

Mesh& AssetStore::Get_Mesh(int index) {
	assert(index <= meshs.size() - 1, "Index out of range in Get_Mesh, AssetStore");
	return meshs[index];
}

std::pair<Mesh&, int> AssetStore::Get_Mesh(std::string path) {
	auto it = pathToIndexMapMesh.find(path);
	
	if (it == pathToIndexMapMesh.end()) {
		meshs.push_back(LoadMesh(path));

		int index = meshs.size() - 1;
		pathToIndexMapMesh[path] = index;
		return std::make_pair(std::ref(meshs[index]), index);
	}
	return std::make_pair(std::ref(meshs[it->second]), it->second);
}

int AssetStore::CheckExistingMat(std::string name) {
	uint32_t hashName = std::hash<std::string>{}(name);
	auto it = pathToIndexMapMaterial.find(hashName);

	if (it == pathToIndexMapMaterial.end()) {
		return -1;
	}
	return it->second;
}


std::pair<Material&, int> AssetStore::CreateMaterial(std::string name, const char* vertexPath, const char* fragmentPath) {
	int index = CheckExistingMat(name);
	if (index != -1) {  // Material already exist
		return std::make_pair(std::ref(materials[index]), index);
	}
	Material material(name, vertexPath, fragmentPath);
	materials.push_back(material);
	int lastElementIndex = materials.size() - 1;
	pathToIndexMapMaterial[std::hash<std::string>{}(name)] = lastElementIndex;

	return std::make_pair(std::ref(materials[lastElementIndex]), lastElementIndex);
}


Material& AssetStore::Get_Material(std::string name) {
	int index = CheckExistingMat(name);

	if (index == -1) {
		std::cout << "Material with the name: " << name << " do not exist in Get_Material(std::string name)" << std::endl;
	}

	return materials[index];
}

Material& AssetStore::Get_Material(int index) {  // Dedicate for the Systemes
	assert(index >= 0, "Negative index detected in Get_Material(int index)");
	assert(index < materials.size(), "Index out of range in Get_Material(int index)");

	return materials[index];
}