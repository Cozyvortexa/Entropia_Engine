#include "ECS/ModelStore.h"

Model ModelStore::LoadModel(std::string path) {
	Model model = Model();
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		abort();  //Add here the default object to report the fail
		//return;  
	}
	//std::cout << "Embedded textures: " << scene->mNumTextures << std::endl;
	//std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;
	//std::cout << "Number of materials: " << scene->mNumMaterials << std::endl;
	model.directory = path.substr(0, path.find_last_of("/"));
	ProcessNode(scene->mRootNode, scene, model);
	return model;
}

void ModelStore::ProcessNode(aiNode* node, const aiScene* scene, Model& currentModel)
{
	// process all the node’s meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		currentModel.meshes.push_back(ProcessMesh(mesh, scene, currentModel));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, currentModel);
	}
}

Mesh ModelStore::ProcessMesh(aiMesh* mesh, const aiScene* scene, Model& currentModel)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		//Position
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		//PosTex
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);


		vertices.push_back(vertex);
	}
	//Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, scene, currentModel);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());


		std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, scene, currentModel);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());


		std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, scene, currentModel);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> ModelStore::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene, Model& currentModel)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		for (unsigned int j = 0; j < currentModel.textures_loaded.size(); j++)
		{
			//std::string fullPath = directory + "/" + std::string(str.C_Str());
			if (std::strcmp(currentModel.textures_loaded[j].path.data(), str.C_Str()) == 0)  // Verifie si la texture a deja etait charger
			{
				textures.push_back(currentModel.textures_loaded[j]);
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
				texture.id = TextureClass::LoadTextureFromFile(str.C_Str(), currentModel.directory);

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
			currentModel.textures_loaded.push_back(texture);

		}

	}
	return textures;
}

Model& ModelStore::Get_Model(int index) {
	assert(index <= models.size() - 1, "Index out of range in Get_Model, ModelStore");
	return models[index];
}

std::pair<Model&, int> ModelStore::Get_Model(std::string path) {
	auto it = pathToIndexMapModel.find(path);
	
	if (it == pathToIndexMapModel.end()) {
		models.push_back(LoadModel(path));

		int index = models.size() - 1;
		pathToIndexMapModel[path] = index;
		return std::make_pair(std::ref(models[index]), index);
	}
	return std::make_pair(std::ref(models[it->second]), it->second);
}

int ModelStore::CheckExistingMat(std::string name) {
	uint32_t hashName = std::hash<std::string>{}(name);
	auto it = pathToIndexMapMaterial.find(hashName);

	if (it == pathToIndexMapMaterial.end()) {
		return -1;
	}
	return it->second;
}


std::pair<Material&, int> ModelStore::CreateMaterial(std::string name, const char* vertexPath, const char* fragmentPath) {
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


Material& ModelStore::Get_Material(std::string name) {
	int index = CheckExistingMat(name);

	if (index == -1) {
		std::cout << "Material with the name: " << name << " do not exist in Get_Material(std::string name)" << std::endl;
	}

	return materials[index];
}

Material& ModelStore::Get_Material(int index) {  // Dedicate for the Systemes
	assert(index >= 0, "Negative index detected in Get_Material(int index)");
	assert(index < materials.size(), "Index out of range in Get_Material(int index)");

	return materials[index];
}