#include "ECS/AssetStore.h"

Mesh AssetStore::LoadMesh(std::string path) {
	Mesh mesh = Mesh();
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | 
		aiProcess_CalcTangentSpace | aiProcess_GlobalScale | aiProcess_PreTransformVertices | aiProcess_GenSmoothNormals);  // WARNING, the flag aiProcess_PreTransformVertices removes the node hierarchy, thereby preventing animations from working 

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
	vertices.reserve(sub_Mesh->mNumVertices);
	std::vector<unsigned int> indices;
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
		//Normal
		if (sub_Mesh->HasNormals()) {
			glm::vec3 normalVec;
			normalVec.x = sub_Mesh->mNormals[i].x;
			normalVec.y = sub_Mesh->mNormals[i].y;
			normalVec.z = sub_Mesh->mNormals[i].z;
			vertex.Normal = normalVec;
		}
		else {
			vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
		}

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
	unsigned int material_Handle = 0;
	if (sub_Mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[sub_Mesh->mMaterialIndex];
		unsigned int diffuseMap_handle = LoadMaterialTextures(material, aiTextureType_BASE_COLOR, scene, currentMesh);
		unsigned int specularMap_handle = LoadMaterialTextures(material, aiTextureType_SPECULAR, scene, currentMesh);
		unsigned int normalMap_handle = LoadMaterialTextures(material, aiTextureType_NORMALS, scene, currentMesh);
		unsigned int ambientOcclusion_handle = LoadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, scene, currentMesh);
		unsigned int metalness_handle = LoadMaterialTextures(material, aiTextureType_METALNESS, scene, currentMesh);
		if (normalMap_handle == -1) {
			normalMap_handle = LoadMaterialTextures(material, aiTextureType_HEIGHT, scene, currentMesh);
			if (normalMap_handle == -1) currentMesh.hasNormalMap = false;
		}
		if (metalness_handle == -1) {
			metalness_handle = LoadMaterialTextures(material, aiTextureType_GLTF_METALLIC_ROUGHNESS, scene, currentMesh);
		}
		if (diffuseMap_handle == -1) {
			diffuseMap_handle = LoadMaterialTextures(material, aiTextureType_DIFFUSE, scene, currentMesh);
		}


		//Create the id key of the mat
		MaterialKey key(diffuseMap_handle, specularMap_handle, normalMap_handle, ambientOcclusion_handle, metalness_handle);
		size_t hash_ID = std::hash<MaterialKey>{}(key);

		auto result = keyTo_MaterialHandle.try_emplace(hash_ID);
		auto it = result.first;
		auto is_Inserted = result.second;


		if (is_Inserted) {  // Create a new material if the key don't exist
			Material material(Get_Material(0)->shader);  // Material at index 0 corresponds to the default mat who has all default value
			material.diffuse_Text_Handle = diffuseMap_handle;
			material.specular_Text_Handle = specularMap_handle;
			material.normal_Text_Handle = normalMap_handle;
			material.ambientOcclusion_Text_Handle = ambientOcclusion_handle;
			material.metalness_handle_Text_Handle = metalness_handle;

			//material.name  // maybe add a custom name

			materials.push_back(material);
			material_Handle = materials.size() - 1;
			it->second = material_Handle;
		}
		else {  // or return the existing material one
			material_Handle = it->second;
		}

	}

	return SubMesh(vertices, indices, material_Handle);
}

unsigned int AssetStore::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene, Mesh& currentMesh)
{
	unsigned int texture_Handle = 0;
	aiString str;
	aiReturn succes = mat->GetTexture(type, 0, &str);  // Only take the first texture
	if (succes == aiReturn_FAILURE) {return -1;}

	auto result = pathToIndexMap_Texture.try_emplace(str.C_Str());
	auto it = result.first;
	auto is_Inserted = result.second;

	if (is_Inserted) { // Create a new texture if the key don't exist
		Texture texture;
		const aiTexture* embeddedTex = scene->GetEmbeddedTexture(str.C_Str());
		if (embeddedTex) {  // texture embarquer detecter
			texture.id = TextureClass::LoadEmbeddedTexture(embeddedTex);
		}
		else
			texture.id = TextureClass::LoadTextureFromFile(str.C_Str(), currentMesh.directory);

		texture.path = str.C_Str();

		switch (type) {  // la texture est charger meme si elle ne serra pas utiliser
		case aiTextureType_DIFFUSE:
			texture.textureType = Texture::Diffuse;
			break;
		case aiTextureType_BASE_COLOR:
			texture.textureType = Texture::Albedo;
			break;
		case aiTextureType_SPECULAR:
			texture.textureType = Texture::Specular;
			break;
		case aiTextureType_NORMALS:
			texture.textureType = Texture::Normal;
			break;
		case aiTextureType_HEIGHT:
			texture.textureType = Texture::Normal;
			break;
		case aiTextureType_METALNESS:
			texture.textureType = Texture::Metalness;
			break;
		case aiTextureType_GLTF_METALLIC_ROUGHNESS:
			texture.textureType = Texture::MetalicRoughness;
			break;
		case aiTextureType_AMBIENT_OCCLUSION:
			texture.textureType = Texture::Ambient_Occlusion;
			break;
		}

		textures.push_back(texture);
		texture_Handle = textures.size() - 1;
		it->second = texture_Handle;
	}
	else { // or return the existing texture one
		texture_Handle = it->second;
	}
	return texture_Handle;
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

Material* AssetStore::Get_Material(unsigned int index) {  // Dedicate to the Systemes
	if (index >= materials.size()) {  // Index out of range in Get_Material(int index)
		return &materials[0];
	}

	return &materials[index];
}
Texture* AssetStore::Get_Texture(unsigned int index) {  // Dedicate to the Systemes
	if (index >= textures.size()) {return nullptr;} // "Index out of range in Get_textures(int index)"
	return &textures[index];
}