#include "Render/Renderer.h"

namespace Component = Engine::Component;


void Renderer::ClearVertexList() {
	mesh_In_VertexList.clear();
	vertexList.clear();
}

void OpenGL_Renderer::AddMeshToRender(Component::MeshHandle newMesh) {
	uint32_t meshIndex = newMesh.index;
	Mesh currentMesh = assetStore->Get_Mesh(meshIndex);

	// Check if the subMesh zero is already load
	uint32_t firstSubMeshKey = (meshIndex << 16) | 0;
	if (mesh_In_VertexList.find(firstSubMeshKey) != mesh_In_VertexList.end()) {
		return;
	}

	for (size_t subMeshIndex = 0; subMeshIndex < currentMesh.subMeshs.size(); ++subMeshIndex) {
		SubMesh subMesh = static_cast<SubMesh>(std::get<OpenGL_SubMesh>(currentMesh.subMeshs[subMeshIndex]));

		uint32_t subMeshKey = (subMeshIndex << 16) | 0;

		//Create new correspondence
		MeshCorrespondence correspondence;
		correspondence.meshIndex = meshIndex;

		correspondence.vertexStart = vertexList.size();
		correspondence.vertexCount = subMesh.vertices.size();

		correspondence.indexStart = indexList.size();
		correspondence.indexCount = subMesh.indices.size();

		// Insertion into global buffers
		vertexList.insert(vertexList.end(), subMesh.vertices.begin(), subMesh.vertices.end());
		indexList.insert(indexList.end(), subMesh.indices.begin(), subMesh.indices.end());


		// Add to history
		mesh_In_VertexList[subMeshKey] = correspondence;
	}

	// Update buffer
	glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexList.size() * sizeof(Vertex), vertexList.data());
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexList.size() * sizeof(uint32_t), indexList.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	assert(vertexList.size() * sizeof(Vertex) <= 200 * 1024 * 1024 && "VBO overflow !");
	assert(indexList.size() * sizeof(uint32_t) <= 100 * 1024 * 1024 && "EBO overflow !");
}

void OpenGL_Renderer::OrderDraw(Component::MeshHandle meshHandle, glm::mat4 modelMatrix) {
	// If the object is not found in the global vertex list, add it
	uint32_t meshIndex = (meshHandle.index << 16) | 0;
	auto it_exist = mesh_In_VertexList.find(meshIndex);
	if (it_exist == mesh_In_VertexList.end()) AddMeshToRender(meshHandle);

	recordedMeshInstances[meshIndex].push_back(modelMatrix);
}

void OpenGL_Renderer::ExecuteRenderCommands() {
	if (recordedMeshInstances.empty()) return;

	//Temporal list
	std::vector<DrawElementsIndirectCommand> indirectCommands;
	std::vector<InstanceData> globalInstanceData;
	indirectCommands.reserve(recordedMeshInstances.size());

	// Iterate over all draw order of this form : (meshIndex, modelMatrix) , previously add by OrderDraw
	// NOTES : meshIndex is the same index used to retrieve a Mesh in the Asset Store 
	for (auto& [meshIndex, matrices] : recordedMeshInstances) {
		Mesh currentMesh = assetStore->Get_Mesh(meshIndex);
		uint32_t instanceCount = static_cast<uint32_t>(matrices.size());

		// Iterate over all subMesh
		for (size_t subMeshIndex = 0; subMeshIndex < currentMesh.subMeshs.size(); ++subMeshIndex) {
			uint32_t subMeshKey = (subMeshIndex << 16) | 0;

			// If the submesh don't existe in the global Vertex, discard it
			auto corrIt = mesh_In_VertexList.find(subMeshKey);
			if (corrIt == mesh_In_VertexList.end()) continue;
			const MeshCorrespondence& corr = corrIt->second;


			OpenGL_SubMesh& subMesh = static_cast<OpenGL_SubMesh&>(std::get<OpenGL_SubMesh>(currentMesh.subMeshs[subMeshIndex]));
			Material* currentMat = assetStore->Get_Material(subMesh.material_Handle);



			// Récupération des handles bindless (avec fallback sur texture par défaut si null)
			Texture* diff = assetStore->Get_Texture(currentMat->diffuse_Text_Handle);
			uint64_t diffH = diff ? diff->bindlessHandle : Shader::GetDefaultText_Handle();

			Texture* norm = assetStore->Get_Texture(currentMat->normal_Text_Handle);
			uint64_t normH = norm ? norm->bindlessHandle : Shader::GetNeutralNormalText_Handle();

			Texture* ao = assetStore->Get_Texture(currentMat->ambientOcclusion_Text_Handle);
			uint64_t aoH = ao ? ao->bindlessHandle : Shader::GetDefaultText_Handle();

			Texture* rough = assetStore->Get_Texture(currentMat->roughness_handle_Text_Handle);
			uint64_t roughH = rough ? rough->bindlessHandle : 0;

			Texture* met = assetStore->Get_Texture(currentMat->metalness_handle_Text_Handle);
			uint64_t metH = met ? met->bindlessHandle : 0;

			// Take the subMesh model matrices
			glm::mat4 localTransform = glm::mat4(1.0f);
			if (subMeshIndex < currentMesh.instancesGroup.size() && !currentMesh.instancesGroup[subMeshIndex].instancedMatrix.empty()) {
				localTransform = currentMesh.instancesGroup[subMeshIndex].instancedMatrix[0];
			}

			// Create a indirect call for this subMesh
			DrawElementsIndirectCommand openGLCmd;
			openGLCmd.count = static_cast<uint32_t>(corr.indexCount);
			openGLCmd.instanceCount = instanceCount;
			openGLCmd.firstIndex = static_cast<uint32_t>(corr.indexStart);
			openGLCmd.baseVertex = static_cast<int32_t>(corr.vertexStart);
			openGLCmd.baseInstance = static_cast<uint32_t>(globalInstanceData.size());

			indirectCommands.push_back(openGLCmd);


			// Apply the modelMatrix and create a InstanceData for each matrices/Submesh instance
			for (const auto& modelMatrix : matrices) {
				InstanceData instData;
				instData.modelMatrix = modelMatrix * localTransform;
				instData.diffuseTexHandle = diffH;
				instData.normalTexHandle = normH;
				instData.aoTexHandle = aoH;
				instData.roughnessTexHandle = roughH;
				instData.metallicTexHandle = metH;
				instData.aoFactor = currentMat->ao_Factor;
				instData.roughnessFactor = currentMat->roughness_Factor;
				instData.metallicFactor = currentMat->metallic_Factor;
				instData.hasRoughness = rough ? 1 : 0;
				instData.hasMetallic = met ? 1 : 0;
				instData.hasNormalMap = currentMesh.hasTBN && currentMesh.hasUV && currentMesh.hasNormalMap;

				globalInstanceData.push_back(instData);
			}

		}
	}


	// A. Envoi des matrices dans le SSBO d'Instances
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, globalInstanceData.size() * sizeof(InstanceData), globalInstanceData.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INSTANCE_BUFFER_BINDING_POINT, instanceSSBO);

	// B. Envoi des commandes dans le Buffer Indirect
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, indirectCommands.size() * sizeof(DrawElementsIndirectCommand), indirectCommands.data(), GL_DYNAMIC_DRAW);

	// Render
	glBindVertexArray(globalVAO);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(indirectCommands.size()), 0);

	// Cleaning
	glBindVertexArray(0);
	recordedMeshInstances.clear();
}

//void OpenGL_Renderer::DrawMesh(Mesh& currentMesh, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) {
//	if (currentMesh.isValid == false) return;
//	if (currentMesh.subMeshs.size() == 0) {
//		std::cout << "The mesh: " << currentMesh.directory << " have no submesh, drawCall cancel" << std::endl;
//		return;
//	}
//	std::vector<std::pair<unsigned int, size_t>> mat_indices;
//	mat_indices.reserve(currentMesh.subMeshs.size());
//
//
//	//Data preparation
//	for (int i = 0; i < currentMesh.subMeshs.size(); i++) {
//		mat_indices.push_back(std::make_pair(std::get<OpenGL_SubMesh>(currentMesh.subMeshs[i]).material_Handle, i));
//	}
//	//Grouping submeshes with identical materials
//	std::sort(mat_indices.begin(), mat_indices.end(), [](const std::pair<unsigned int, size_t>& a, const std::pair<unsigned int, size_t>& b) {
//		return a.first < b.first; 
//	});
//
//
//	int lastHandle = mat_indices[0].first;
//	Material* last_Material_Use = assetStore->Get_Material(lastHandle);
//	Shader* last_Shader_Use = &last_Material_Use->shader;
//	last_Shader_Use->Use();
//
//	//Draw
//	for (std::pair<unsigned int, size_t>& current_Material_Handle : mat_indices) {
//
//		if (lastHandle != current_Material_Handle.first) {  // New material detected
//			lastHandle = current_Material_Handle.first;
//
//			last_Material_Use = assetStore->Get_Material(lastHandle);
//			last_Shader_Use = &last_Material_Use->shader;
//			last_Shader_Use->Use();
//		}
//		size_t subMeshIndex = current_Material_Handle.second;
//
//		OpenGL_SubMesh& subMesh = static_cast<OpenGL_SubMesh&>(std::get<OpenGL_SubMesh>(currentMesh.subMeshs[subMeshIndex]));
//
//
//		assert(glIsVertexArray(subMesh.VAO));
//		assert(glIsBuffer(subMesh.EBO));
//
//		Material* currentMat = assetStore->Get_Material(subMesh.material_Handle);
//
//		bool hasRoughness = false;
//		bool hasMetallic = false;
//
//		unsigned int diffuse_Text = 0;  // diffuse or Albedo
//		unsigned int normal_Text = 0;
//		unsigned int ambientOcclusion_Text = 0;
//		unsigned int roughness_Text = 0;
//		unsigned int metallic_Text = 0;
//
//		// Get texture
//		// 
//		//Diffuse
//		Texture* diffuse = assetStore->Get_Texture(currentMat->diffuse_Text_Handle);
//		if (diffuse == nullptr) {
//			diffuse_Text = Shader::GetDefaultText();
//		}
//		else { diffuse_Text = diffuse->id; }
//
//		//Normal
//		Texture* normal = assetStore->Get_Texture(currentMat->normal_Text_Handle);
//		if (normal == nullptr) {
//			normal_Text = Shader::GetNeutralNormalText();
//		}
//		else { normal_Text = normal->id; }
//
//		//Ambient occlusion
//		Texture* ambientOcclusion = assetStore->Get_Texture(currentMat->ambientOcclusion_Text_Handle);
//		if (ambientOcclusion == nullptr) {
//			ambientOcclusion_Text = Shader::GetDefaultText();
//		}
//		else { ambientOcclusion_Text = ambientOcclusion->id; }
//
//		//Roughness
//		Texture* roughness = assetStore->Get_Texture(currentMat->roughness_handle_Text_Handle);
//		if (roughness != nullptr) {
//			hasRoughness = true;
//		}
//		//Metallic
//		Texture* metallic = assetStore->Get_Texture(currentMat->metalness_handle_Text_Handle);
//		if (metallic != nullptr) {
//			hasMetallic = true;
//		}
//
//		// --- Link Matrices ---
//		last_Shader_Use->setMatrix("view", viewMatrix);
//		last_Shader_Use->setMatrix("projection", projectionMatrix);
//
//
//		// Bind
//		int i = 0;
//		//Diffuse/Albedo
//		glActiveTexture(GL_TEXTURE0 + i);
//		glBindTexture(GL_TEXTURE_2D, diffuse_Text);
//		last_Shader_Use->setInt("material.diffuseText", i++);
//
//		glActiveTexture(GL_TEXTURE0 + i);
//		glBindTexture(GL_TEXTURE_2D, normal_Text);
//		last_Shader_Use->setInt("material.normalText", i++);
//		last_Shader_Use->setBool("have_NormalMap", currentMesh.hasTBN && currentMesh.hasUV && currentMesh.hasNormalMap);
//
//		glActiveTexture(GL_TEXTURE0 + i);
//		glBindTexture(GL_TEXTURE_2D, ambientOcclusion_Text);
//		last_Shader_Use->setInt("material.ambientOcclusion_Text", i++);
//
//		last_Shader_Use->setBool("material.hasRoughness_Text", hasRoughness);
//		if (hasRoughness) {
//			glActiveTexture(GL_TEXTURE0 + i);
//			glBindTexture(GL_TEXTURE_2D, roughness_Text);
//			last_Shader_Use->setInt("material.roughness_Text", i++);
//		}
//		last_Shader_Use->setBool("material.hasMetallic_Text", hasMetallic);
//		if (hasMetallic) {
//			glActiveTexture(GL_TEXTURE0 + i);
//			glBindTexture(GL_TEXTURE_2D, metallic_Text);
//			last_Shader_Use->setInt("material.metallic_Text", i++);
//		}
//
//
//		//ARM
//		//last_Shader_Use->setBool("material.hasARM_Text", currentMat->hasARM_Text);
//		last_Shader_Use->setFloat("material.ao_Factor", currentMat->ao_Factor);
//		last_Shader_Use->setFloat("material.roughness_Factor", currentMat->roughness_Factor);
//		last_Shader_Use->setFloat("material.metallic_Factor", currentMat->metallic_Factor);
//
//		glActiveTexture(GL_TEXTURE0);
//
//
//		glBindVertexArray(subMesh.VAO);
//
//		// The subMesh have instanced subMesh to draw
//		if (subMeshIndex < currentMesh.instancesGroup.size() && currentMesh.instancesGroup[subMeshIndex].instancedMatrix.size() > 1) {
//			auto& group = currentMesh.instancesGroup[subMeshIndex];
//
//			glBindBuffer(GL_ARRAY_BUFFER, group.instanceMatrixVBO);
//			glBufferData(GL_ARRAY_BUFFER, group.instancedMatrix.size() * sizeof(glm::mat4), group.instancedMatrix.data(), GL_DYNAMIC_DRAW);
//
//			// Configure the vertex attributes for the 4x4 matrix (locations 4, 5, 6, 7)
//			// A mat4 is equivalent to 4 consecutive vec4
//			std::size_t vec4Size = sizeof(glm::vec4);
//			for (unsigned int i = 0; i < 4; i++) {
//				glEnableVertexAttribArray(4 + i);
//				glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
//				glVertexAttribDivisor(4 + i, 1); // Advance per instance
//			}
//			// Draw instanced SubMeshs
//			glDrawElementsInstanced(GL_TRIANGLES, subMesh.indices.size(), GL_UNSIGNED_INT, 0, group.instancedMatrix.size());
//
//			// Clearing instance attributes
//			for (unsigned int i = 0; i < 4; i++) {
//				glDisableVertexAttribArray(4 + i);
//			}
//		}
//		else {
//			last_Shader_Use->setMatrix("model", modelMatrix * currentMesh.instancesGroup[subMeshIndex].instancedMatrix[0]);
//			// Draw SubMesh
//			glDrawElements(GL_TRIANGLES, subMesh.indices.size(), GL_UNSIGNED_INT, 0);
//		}
//
//		glBindVertexArray(0);
//	}
//}

void OpenGL_Renderer::DrawMesh_Without_Texture(Mesh& currentMesh) {
	if (currentMesh.isValid == false) return;
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

void OpenGL_Renderer::SetupAxisArrow() {
	const float arrowVertices[] = {
		// Arrows body's
		0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		// Tip (left/right)
		0.0f, 1.0f, 0.0f,  -0.1f, 0.8f, 0.0f,
		0.0f, 1.0f, 0.0f,   0.1f, 0.8f, 0.0f
	};


	glGenVertexArrays(1, &arrowVAO);
	glGenBuffers(1, &arrowVBO);

	glBindVertexArray(arrowVAO);
	glBindBuffer(GL_ARRAY_BUFFER, arrowVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(arrowVertices), arrowVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void OpenGL_Renderer::RenderAxisGizmo(glm::vec3 objectPosition, glm::mat4 view, glm::mat4 projection, Shader& gizmoShader){
	gizmoShader.Use();
	gizmoShader.setMatrix("view", view);
	gizmoShader.setMatrix("projection", projection);


	glDisable(GL_DEPTH_TEST);
	glLineWidth(3.0f);

	glBindVertexArray(arrowVAO);

	glm::mat4 model;

	// --- AXIS Y (GREEN) ---
	model = glm::mat4(1.0f);
	model = glm::translate(model, objectPosition);
	model = glm::scale(model, glm::vec3(1.5f));
	gizmoShader.setMatrix("model", model);
	gizmoShader.setVec("axisColor", glm::vec3(0.0f, 1.0f, 0.0f));
	glDrawArrays(GL_LINES, 0, 6);

	// --- AXIS X (RED) ---
	model = glm::mat4(1.0f);
	model = glm::translate(model, objectPosition);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.5f));
	gizmoShader.setMatrix("model", model);
	gizmoShader.setVec("axisColor", glm::vec3(1.0f, 0.0f, 0.0f));
	glDrawArrays(GL_LINES, 0, 6);

	// --- AXIS Z (BLUE) ---
	model = glm::mat4(1.0f);
	model = glm::translate(model, objectPosition);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.5f));
	gizmoShader.setMatrix("model", model);
	gizmoShader.setVec("axisColor", glm::vec3(0.0f, 0.0f, 1.0f));
	glDrawArrays(GL_LINES, 0, 6);

	glBindVertexArray(0);
	glLineWidth(1.0f);
	glEnable(GL_DEPTH_TEST);
}


void OpenGL_Renderer::SetViewport_Size(glm::vec2 newSize) {
	if (newSize.x < 0 || newSize.y < 0) return;
	glViewport(0, 0, newSize.x, newSize.y);
}

//Call when the viewport is re-scall
void OpenGL_Renderer::ResizeFrameBufferText(Engine::Resource::RenderResource* renderData) {
	if (renderData->renderWIDTH < 0 || renderData->renderHEIGHT < 0) return;
	glBindTexture(GL_TEXTURE_2D, renderData->finalDepthOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, renderData->finalTxtColorOutput[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, renderData->finalTxtColorOutput[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

	//Gbuffer
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gARM);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	//Bloom
	glBindTexture(GL_TEXTURE_2D, renderData->pingpongBuffers[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, renderData->pingpongBuffers[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	//SSAO
	glBindTexture(GL_TEXTURE_2D, renderData->ssao.ssaoText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	//SSAO-Blur
	glBindTexture(GL_TEXTURE_2D, renderData->ssao.ssaoBlurText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RED, GL_FLOAT, NULL);

	//Imgui
	glBindTexture(GL_TEXTURE_2D, renderData->toImGui_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}