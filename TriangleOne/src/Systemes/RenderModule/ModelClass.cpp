#include <Systemes/RenderModule/ModelClass.h>

void Model::LoadModel(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	//std::cout << "Embedded textures: " << scene->mNumTextures << std::endl;
	//std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;
	//std::cout << "Number of materials: " << scene->mNumMaterials << std::endl;
	directory = path.substr(0, path.find_last_of("/"));
	ProcessNode(scene->mRootNode, scene);

}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	// process all the node’s meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
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

		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());


		std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, scene);
		textures.insert(textures.end(), specularMaps.begin(),specularMaps.end());


		std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, scene);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			//std::string fullPath = directory + "/" + std::string(str.C_Str());
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)  // Verifie si la texture a deja etait charger
			{
				textures.push_back(textures_loaded[j]);
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
				texture.id = TextureClass::LoadTextureFromFile(str.C_Str(), directory);

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
			textures_loaded.push_back(texture);

		}

	}
	return textures;
}

void Model::Draw(std::shared_ptr<Shader> shader) {
	for (Mesh& mesh : meshes)
		mesh.Draw(shader);
}
void Model::DrawWithoutTexture(std::shared_ptr<Shader> shader){
	for (Mesh& mesh : meshes)
		mesh.DrawWithoutTexture(shader);
}

