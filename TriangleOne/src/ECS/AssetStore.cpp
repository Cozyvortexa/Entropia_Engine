#include "ECS/AssetStore.h"

namespace Resource = Engine::Resource;
namespace Audio = Engine::Audio;

static glm::mat4 AssimpMat4_To_glmMat4(aiMatrix4x4& assimpMat4) {
	glm::mat4 nodeTransform;
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			nodeTransform[y][x] = assimpMat4[x][y];
		}
	}
	return nodeTransform;
}

#pragma region Texture

std::shared_ptr<Texture> AssetStore::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene, Mesh& currentMesh, std::string path){
	aiString str;
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();

	aiReturn succes = mat->GetTexture(type, 0, &str);  // Only take the first texture
	if (succes == aiReturn_FAILURE) {
		texture->id = (unsigned int)TextureLoadState::NotFound;
		return texture;
	}

	std::string keyPath = str.C_Str();
	// embedded path detected
	if (keyPath.size() > 0 && keyPath[0] == '*') {
		keyPath = path + str.C_Str();
	}
	else {
		keyPath = (ASSETS_DIR / currentMesh.directory / keyPath).string();
	}

	auto result = pathToIndexMap_Texture.try_emplace(keyPath);
	auto it = result.first;
	auto is_Inserted = result.second;

	if (is_Inserted) { // Create a new texture if the key don't exist
		texture->path = keyPath;

		switch (type) {
		case aiTextureType_DIFFUSE:
			texture->textureType = Texture::Diffuse;
			break;
		case aiTextureType_BASE_COLOR:
			texture->textureType = Texture::Albedo;
			break;
		case aiTextureType_SPECULAR:
			texture->textureType = Texture::Specular;
			break;
		case aiTextureType_NORMALS:
			texture->textureType = Texture::Normal;
			break;
		case aiTextureType_HEIGHT:
			texture->textureType = Texture::Normal;
			break;
		case aiTextureType_METALNESS:
			texture->textureType = Texture::Metalness;
			break;
		case aiTextureType_GLTF_METALLIC_ROUGHNESS:
			texture->textureType = Texture::MetalicRoughness;
			break;
		case aiTextureType_AMBIENT_OCCLUSION:
			texture->textureType = Texture::Ambient_Occlusion;
			break;
		}

		const aiTexture* embeddedTex = scene->GetEmbeddedTexture(str.C_Str());
		if (embeddedTex) {  // embedded texture detected
			texture->id = TextureClass::LoadEmbeddedTexture(embeddedTex);

			texture->bindlessHandle = glGetTextureHandleARB(texture->id);
			//Makes the texture persistent in GPU memory
			glMakeTextureHandleResidentARB(texture->bindlessHandle);
		}
		else {
			// --- MULTI-THREADING FOR DISK-BASED TEXTURES ---
			texture->bindlessHandle = 0; // Will be generate later on the main thread !

			std::string filename = str.C_Str();
			std::string dir = currentMesh.directory;

			// Launch the loading process in the background
			m_PendingTextures.push_back(std::async(std::launch::async, [filename, dir, keyPath, type]() {
				PendingTextureData pendingData;
				pendingData.keyPath = keyPath;
				pendingData.type = type;

				std::string totalPath = ASSETS_DIR.string() + "/" + dir + "/" + filename;

				pendingData.data = stbi_load(totalPath.c_str(), &pendingData.width, &pendingData.height, &pendingData.nrChannels, 0);
				return pendingData;
				}));

		}

		texture->id = (unsigned int)TextureLoadState::Pending;
		textures.push_back(texture);
		it->second = texture;
		return texture;
	}
	else { // or return the existing texture one
		return it->second.lock();
	}
}

std::shared_ptr<Texture> AssetStore::Get_Texture(unsigned int index) {  // Dedicate to the Systemes
	if (index >= textures.size()) { return nullptr; } // "Index out of range in Get_textures(int index)"
	return textures[index];
}


#pragma endregion

#pragma region Material

int AssetStore::CheckExistingMat(std::string name) {
	uint32_t hashName = std::hash<std::string>{}(name);
	auto it = pathToIndexMapMaterial.find(hashName);

	if (it == pathToIndexMapMaterial.end()) {
		return -1;
	}
	return it->second;
}

std::pair<std::shared_ptr<Material>, int> AssetStore::CreateMaterial(std::string name, const char* vertexPath, const char* fragmentPath) {
	int index = CheckExistingMat(name);
	if (index != -1) {  // Material already exist
		return std::make_pair(std::ref(materials[index]), index);
	}
	std::shared_ptr<Material> material = std::make_shared<Material>(name, vertexPath, fragmentPath);
	materials.push_back(material);
	int lastElementIndex = materials.size() - 1;
	pathToIndexMapMaterial[std::hash<std::string>{}(name)] = lastElementIndex;

	return std::make_pair(std::ref(materials[lastElementIndex]), lastElementIndex);
}

std::shared_ptr<Material> AssetStore::Get_Material(std::string name) {
	int index = CheckExistingMat(name);

	if (index == -1) {
		std::cout << "Material with the name: " << name << " do not exist in Get_Material(std::string name)" << std::endl;
		abort();
	}

	return materials[index];
}

std::shared_ptr<Material> AssetStore::Get_Material(unsigned int index) {
	assert(index >= 0);
	if (index >= materials.size()) {
		return materials[0];
	}

	return materials[index];
}

#pragma endregion

#pragma region Mesh

//Count the number of identical meshes (Instancing purpose)
void AssetStore::CountMesh(std::unordered_map<unsigned int, std::vector<aiNode*>>& meshCounts, aiNode* node) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		meshCounts[node->mMeshes[i]].push_back(node);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		CountMesh(meshCounts, node->mChildren[i]);
	}
}

std::shared_ptr<Mesh> AssetStore::LoadMesh(std::string path) {
	std::filesystem::path mesh_FullPath = ASSETS_DIR / path.c_str();

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(mesh_FullPath.string(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace | aiProcess_GlobalScale | aiProcess_GenSmoothNormals |  aiProcess_FindInstances );

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		abort();  //Add here the default object to report the fail
		//return;  
	}
	//std::cout << "Embedded textures: " << scene->mNumTextures << std::endl;
	//std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;
	//std::cout << "Number of materials: " << scene->mNumMaterials << std::endl;
	mesh->directory = path.substr(0, path.find_last_of("/"));

	CountMesh(meshCounts, scene->mRootNode);

	ProcessNode(scene->mRootNode, scene, *mesh, mesh_FullPath.string());

	//Last Step
	for (auto& futureTexture : m_PendingTextures) {
		// Blocks the current thread until stbi_load has finished for this texture
		PendingTextureData data = futureTexture.get();

		if (data.data) {
			Texture* texture = pathToIndexMap_Texture[data.keyPath].lock().get();

			unsigned int textureID = TextureClass::Load_OpenGL_Texture(data.data, data.width, data.height, data.nrChannels);
			stbi_image_free(data.data);


			// Updates the OpenGL ID of the texture that was waiting
			texture->id = textureID;
			texture->bindlessHandle = glGetTextureHandleARB(textureID);

			//Makes the texture persistent in GPU memory
			glMakeTextureHandleResidentARB(texture->bindlessHandle);
		}
		else {
			std::cout << "fail to load the texture in async thread: " << data.keyPath << std::endl;
		}
	}

	m_PendingTextures.clear();
	meshCounts.clear();
	assimpToOurMeshIndex.clear();
	mesh->isValid = true;

	mesh->uniqueMeshIndex = uniqueMeshIndexCompteur++;
	meshs.push_back(mesh);
	return mesh;
}

void AssetStore::ProcessNode(aiNode* node, const aiScene* scene, Mesh& currentMesh, std::string path, glm::mat4 parentTransform){

	glm::mat4 nodeTransform = AssimpMat4_To_glmMat4(node->mTransformation);
	glm::mat4 globalTransform = parentTransform * nodeTransform;


	// process all the node’s meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++){
		aiMesh* subMesh = scene->mMeshes[node->mMeshes[i]];


		// Global index in the assimp scene
		unsigned int assimpMeshIndex = node->mMeshes[i];
		auto it = assimpToOurMeshIndex.find(assimpMeshIndex);

		bool subMeshAreInstanced = meshCounts[assimpMeshIndex].size() > 1;
		if (it == assimpToOurMeshIndex.end()) {
			ProcessSub_Mesh(subMesh, scene, currentMesh, path, subMeshAreInstanced);
			assimpToOurMeshIndex[assimpMeshIndex] = currentMesh.subMeshs.size() - 1;

			InstanceGroup group;
			currentMesh.instancesGroup.push_back(group);
		}

		size_t ourMeshIndex = assimpToOurMeshIndex[assimpMeshIndex];
		if (subMeshAreInstanced) {
			currentMesh.instancesGroup[ourMeshIndex].instancedMatrix.push_back(globalTransform);
		}
		else if (currentMesh.instancesGroup[ourMeshIndex].instancedMatrix.empty()) {
			currentMesh.instancesGroup[ourMeshIndex].instancedMatrix.push_back(globalTransform);
		}
	}
	// Then we do the same for each of its childrens
	for (unsigned int i = 0; i < node->mNumChildren; i++){
		ProcessNode(node->mChildren[i], scene, currentMesh, path, globalTransform);
	}
}

void AssetStore::ProcessSub_Mesh(aiMesh* sub_Mesh, const aiScene* scene, Mesh& currentMesh, std::string path, bool instancedSubMesh){
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
	std::shared_ptr<Material> material = std::make_shared<Material>();
	if (sub_Mesh->mMaterialIndex >= 0)
	{
		aiMaterial* assimp_material = scene->mMaterials[sub_Mesh->mMaterialIndex];
		std::shared_ptr<Texture> diffuseMap_text = LoadMaterialTextures(assimp_material, aiTextureType_BASE_COLOR, scene, currentMesh, path);
		std::shared_ptr<Texture> normalMap_text = LoadMaterialTextures(assimp_material, aiTextureType_NORMALS, scene, currentMesh, path);
		std::shared_ptr<Texture> ambientOcclusion_text = LoadMaterialTextures(assimp_material, aiTextureType_AMBIENT_OCCLUSION, scene, currentMesh, path);
		std::shared_ptr<Texture> metalness_text = LoadMaterialTextures(assimp_material, aiTextureType_METALNESS, scene, currentMesh, path);
		std::shared_ptr<Texture> roughness_text = LoadMaterialTextures(assimp_material, aiTextureType_DIFFUSE_ROUGHNESS, scene, currentMesh, path);

		const unsigned int NOT_FOUND = (unsigned int)TextureLoadState::NotFound;
		if (normalMap_text->id == NOT_FOUND) {
			normalMap_text = LoadMaterialTextures(assimp_material, aiTextureType_HEIGHT, scene, currentMesh, path);
			if (normalMap_text->id == NOT_FOUND) currentMesh.hasNormalMap = false;
		}
		//if (metalness_id == -1) {
		//	metalness_id = LoadMaterialTextures(assimp_material, aiTextureType_GLTF_METALLIC_ROUGHNESS, scene, currentMesh);
		//}
		if (diffuseMap_text->id == NOT_FOUND) {
			diffuseMap_text = LoadMaterialTextures(assimp_material, aiTextureType_DIFFUSE, scene, currentMesh, path);
		}

		//Create the id key of the mat
		MaterialKey key(diffuseMap_text.get(), normalMap_text.get(), ambientOcclusion_text.get(), metalness_text.get(), roughness_text.get());
		size_t hash_ID = std::hash<MaterialKey>{}(key);

		auto result = keyTo_MaterialHandle.try_emplace(hash_ID);
		auto it = result.first;
		auto is_Inserted = result.second;


		if (is_Inserted) {  // Create a new material if the key don't exist
			if (instancedSubMesh) material->shader = Get_Material(1)->shader;
			else material->shader = Get_Material(0)->shader; // Material at index 0 corresponds to the default mat who has all default value

			material->diffuse_Text_Ptr = diffuseMap_text;
			material->normal_Text_Ptr = normalMap_text;
			material->ambientOcclusion_Text_Ptr = ambientOcclusion_text;
			material->metalness_handle_Text_Ptr = metalness_text;
			material->roughness_handle_Text_Ptr = roughness_text;

			assimp_material->Get(AI_MATKEY_METALLIC_FACTOR, material->metallic_Factor);
			assimp_material->Get(AI_MATKEY_ROUGHNESS_FACTOR, material->roughness_Factor);

			//material->name  // maybe add a custom name

			materials.push_back(material);
			it->second = material;
		}
		else {  // or return the existing material one
			material = it->second.lock();
		}

	}
	currentMesh.Create_SubMesh(vertices, indices, material);
}

//std::shared_ptr<Mesh> AssetStore::Get_Mesh(int index) {
//	assert(index <= meshs.size() - 1 && "Index out of range in Get_Mesh, AssetStore");
//	return meshs[index];
//}

// Relative path ONLY (Assets Dir)
std::shared_ptr<Mesh> AssetStore::Get_Mesh(std::string path) {
	auto it = meshPath_AlreadyLoad.find(path);

	if (it == meshPath_AlreadyLoad.end()) {
		std::shared_ptr<Mesh> mesh = LoadMesh(path);
		meshPath_AlreadyLoad[path] = mesh;

		return mesh;
	}
	return it->second.lock();
}

void AssetStore::CheckNonUseResources() {
	//Meshs
	meshs.erase(
		std::remove_if(meshs.begin(), meshs.end(),
			[](const std::shared_ptr<Mesh>& mesh){
				return mesh.use_count() == 1;
			}),
		meshs.end()
	);
	////Materials
	//materials.erase(
	//	std::remove_if(materials.begin(), materials.end(),
	//		[](const std::shared_ptr<Material>& material) {
	//			return material.use_count() == 1;
	//		}),
	//	materials.end()
	//);
	////Textures
	//textures.erase(
	//	std::remove_if(textures.begin(), textures.end(),
	//		[](const std::shared_ptr<Texture>& textures) {
	//			return textures.use_count() == 1;
	//		}),
	//	textures.end()
	//);
}

#pragma endregion

#pragma region Sound

/*
*@brief Creates a sound and adds it to the sound Bank
*
* @param name : name of the Audio
*
* @param path : path to the audio file
*
* @param loadMethod : define the load method (SYNCHRONE, ASYNCHRONE)
*
* @param readerMethode : defines how the audio file is played (STREAMING, PRELOADING)
*
*/
Audio::Audio* AssetStore::Load_Sound(ma_engine& audioEngine, const std::string& name, const char* path, Audio::SoundFlags flags) {

	ma_sound_flags readMethod = Audio::HasFlag(flags, Audio::SoundFlags::Preload) ? MA_SOUND_FLAG_DECODE : MA_SOUND_FLAG_STREAM;
	ma_sound_flags loadMethod = Audio::HasFlag(flags, Audio::SoundFlags::Async) ? MA_SOUND_FLAG_ASYNC : MA_SOUND_FLAG_WAIT_INIT;

	ma_sound* sound = new ma_sound;


	ma_result result = ma_sound_init_from_file(
		&audioEngine,
		path,
		loadMethod | readMethod,
		nullptr, nullptr, sound
	);

	if (result != MA_SUCCESS) {
		delete sound;
		return nullptr;
	}

	return new Audio::Audio(name, path, sound, flags);
}

#pragma endregion