#include "Render/Renderer.h"

void Renderer::DrawMesh(Mesh& currentMesh) {
	if (currentMesh.subMeshs.size() == 0) {
		std::cout << "The mesh: " << currentMesh.directory << " have no submesh, drawCall cancel" << std::endl;
		return;
	}
	std::vector<std::pair<unsigned int, size_t>> mat_indices;
	mat_indices.reserve(currentMesh.subMeshs.size());


	//Data preparation
	for (int i = 0; i < currentMesh.subMeshs.size(); i++) {
		mat_indices.push_back(std::make_pair(currentMesh.subMeshs[i].material_Handle, i));
	}
	//Grouping submeshes with identical materials
	std::sort(mat_indices.begin(), mat_indices.end(), [](const std::pair<unsigned int, size_t>& a, const std::pair<unsigned int, size_t>& b) {
		return a.first < b.first; 
	});


	int lastHandle = mat_indices[0].first;
	Material* last_Material_Use = assetStore->Get_Material(lastHandle);
	Shader* last_Shader_Use = &last_Material_Use->shader;
	last_Shader_Use->Use();

	//Draw
	for (std::pair<unsigned int, size_t>& current_Material_Handle : mat_indices) {

		if (lastHandle != current_Material_Handle.first) {  // New material detected
			lastHandle = current_Material_Handle.first;

			last_Material_Use = assetStore->Get_Material(lastHandle);
			last_Shader_Use = &last_Material_Use->shader;
			last_Shader_Use->Use();
		}
		SubMesh& subMesh = currentMesh.subMeshs[current_Material_Handle.second];


		assert(glIsVertexArray(subMesh.VAO));
		assert(glIsBuffer(subMesh.EBO));

		Material* currentMat = assetStore->Get_Material(subMesh.material_Handle);

		bool haveSpecular = true;

		unsigned int diffuse_Text = 0;
		unsigned int specular_Text = 0;
		unsigned int normal_Text = 0;

		// Get texture
		// 
		//Diffuse
		Texture* diffuse = assetStore->Get_Texture(currentMat->diffuse_Text_Handle);
		if (diffuse == nullptr) {
			diffuse_Text = Shader::GetDefaultText();
		}
		else { diffuse_Text = diffuse->id; }

		//Specular
		Texture* specular = assetStore->Get_Texture(currentMat->specular_Text_Handle);
		if (specular == nullptr) {
			haveSpecular = false;
			specular_Text = Shader::GetDefaultText(); // Just to put something in the slot
		}
		else { specular_Text = specular->id; }

		//Normal
		Texture* normal = assetStore->Get_Texture(currentMat->normal_Text_Handle);
		if (normal == nullptr) {
			normal_Text = Shader::GetNeutralNormalText();
		}
		else { normal_Text = normal->id; }


		// Bind
		int i = 0;
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, diffuse_Text);
		last_Shader_Use->setInt("material.diffuseText", i);
		i++;

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, specular_Text);
		last_Shader_Use->setInt("material.specularText", i);
		last_Shader_Use->setBool("have_Specular", haveSpecular);
		i++;

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, normal_Text);
		last_Shader_Use->setInt("material.normalText", i);
		last_Shader_Use->setBool("have_NormalMap", currentMesh.hasTBN && currentMesh.hasUV && currentMesh.hasNormalMap);
		i++;

		glActiveTexture(GL_TEXTURE0);


		// draw SubMesh
		glBindVertexArray(subMesh.VAO);
		glDrawElements(GL_TRIANGLES, subMesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Renderer::DrawMesh_Without_Texture(Mesh& currentMesh) {
	for (SubMesh& subMesh : currentMesh.subMeshs) {
		glBindVertexArray(subMesh.VAO);
		glDrawElements(GL_TRIANGLES, subMesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Renderer::DrawQuad(RenderResource* renderData) {
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(renderData->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawCube() {

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Renderer::LoadDefaultCube() {
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}