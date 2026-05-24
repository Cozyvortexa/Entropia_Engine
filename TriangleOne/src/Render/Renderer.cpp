#include "Render/Renderer.h"

void OpenGL_Renderer::DrawMesh(Mesh& currentMesh) {
	if (currentMesh.subMeshs.size() == 0) {
		std::cout << "The mesh: " << currentMesh.directory << " have no submesh, drawCall cancel" << std::endl;
		return;
	}
	std::vector<std::pair<unsigned int, size_t>> mat_indices;
	mat_indices.reserve(currentMesh.subMeshs.size());


	//Data preparation
	for (int i = 0; i < currentMesh.subMeshs.size(); i++) {
		mat_indices.push_back(std::make_pair(std::get<OpenGL_SubMesh>(currentMesh.subMeshs[i]).material_Handle, i));
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
		OpenGL_SubMesh& subMesh = static_cast<OpenGL_SubMesh&>(std::get<OpenGL_SubMesh>(currentMesh.subMeshs[current_Material_Handle.second]));


		assert(glIsVertexArray(subMesh.VAO));
		assert(glIsBuffer(subMesh.EBO));

		Material* currentMat = assetStore->Get_Material(subMesh.material_Handle);

		bool hasRoughness = false;
		bool hasMetallic = false;

		unsigned int diffuse_Text = 0;  // diffuse or Albedo
		unsigned int normal_Text = 0;
		unsigned int ambientOcclusion_Text = 0;
		unsigned int roughness_Text = 0;
		unsigned int metallic_Text = 0;

		// Get texture
		// 
		//Diffuse
		Texture* diffuse = assetStore->Get_Texture(currentMat->diffuse_Text_Handle);
		if (diffuse == nullptr) {
			diffuse_Text = Shader::GetDefaultText();
		}
		else { diffuse_Text = diffuse->id; }

		//Normal
		Texture* normal = assetStore->Get_Texture(currentMat->normal_Text_Handle);
		if (normal == nullptr) {
			normal_Text = Shader::GetNeutralNormalText();
		}
		else { normal_Text = normal->id; }

		//Ambient occlusion
		Texture* ambientOcclusion = assetStore->Get_Texture(currentMat->ambientOcclusion_Text_Handle);
		if (ambientOcclusion == nullptr) {
			ambientOcclusion_Text = Shader::GetDefaultText();
		}
		else { ambientOcclusion_Text = ambientOcclusion->id; }

		//Roughness
		Texture* roughness = assetStore->Get_Texture(currentMat->roughness_handle_Text_Handle);
		if (roughness != nullptr) {
			hasRoughness = true;
		}
		//Metallic
		Texture* metallic = assetStore->Get_Texture(currentMat->metalness_handle_Text_Handle);
		if (metallic != nullptr) {
			hasMetallic = true;
		}


		// Bind
		int i = 0;
		//Diffuse/Albedo
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, diffuse_Text);
		last_Shader_Use->setInt("material.diffuseText", i++);

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, normal_Text);
		last_Shader_Use->setInt("material.normalText", i++);
		last_Shader_Use->setBool("have_NormalMap", currentMesh.hasTBN && currentMesh.hasUV && currentMesh.hasNormalMap);

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, ambientOcclusion_Text);
		last_Shader_Use->setInt("material.ambientOcclusion_Text", i++);

		last_Shader_Use->setBool("material.hasRoughness_Text", hasRoughness);
		if (hasRoughness) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, roughness_Text);
			last_Shader_Use->setInt("material.roughness_Text", i++);
		}
		last_Shader_Use->setBool("material.hasMetallic_Text", hasMetallic);
		if (hasMetallic) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, metallic_Text);
			last_Shader_Use->setInt("material.metallic_Text", i++);
		}


		//ARM
		//last_Shader_Use->setBool("material.hasARM_Text", currentMat->hasARM_Text);
		last_Shader_Use->setFloat("material.ao_Factor", currentMat->ao_Factor);
		last_Shader_Use->setFloat("material.roughness_Factor", currentMat->roughness_Factor);
		last_Shader_Use->setFloat("material.metallic_Factor", currentMat->metallic_Factor);
		//if (currentMat->hasARM_Text) {  
		//	last_Shader_Use->setInt("material.AO_Text", i);
		//}

		glActiveTexture(GL_TEXTURE0);


		// draw SubMesh
		glBindVertexArray(subMesh.VAO);
		glDrawElements(GL_TRIANGLES, subMesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void OpenGL_Renderer::DrawMesh_Without_Texture(Mesh& currentMesh) {
	for (auto& subMesh : currentMesh.subMeshs) {
		OpenGL_SubMesh& openGL_subMesh = std::get<OpenGL_SubMesh>(subMesh); 

		glBindVertexArray(openGL_subMesh.VAO);
		glDrawElements(GL_TRIANGLES, openGL_subMesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void OpenGL_Renderer::DrawQuad(Engine::Resource::RenderResource* renderData) {
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(renderData->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void OpenGL_Renderer::DrawCube() {

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void OpenGL_Renderer::LoadDefaultCube() {
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


void OpenGL_Renderer::SetViewport_Size(glm::vec2 newSize) {
	glViewport(0, 0, newSize.x, newSize.y);
}