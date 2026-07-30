#include "Render/Renderer.h"

namespace Component = Engine::Component;

//Renderer
void Renderer::ClearVertexList() {
	mesh_In_VertexList.clear();
	vertexList.clear();
}

void Renderer::ClearOrderList() {
	recordedMeshInstances.clear();
}

void Renderer::ClearShadowOrderList() {
	recordedMeshInstances_ShadowPass.clear();
	shadow_IndirectCommands.clear();
}

//OpenGl

OpenGL_Renderer::OpenGL_Renderer(AssetStore* assetStore) : Renderer(assetStore) {
	LoadDefaultCube();
	SetupAxisArrow();

	glGenVertexArrays(1, &globalVAO);
	glGenBuffers(1, &globalVBO);
	glGenBuffers(1, &globalEBO);

	glBindVertexArray(globalVAO);

	glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
	glBufferData(GL_ARRAY_BUFFER, 100 * 1024 * 1024, nullptr, GL_STATIC_DRAW);

	// Layout 0 : Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	// Layout 1 : Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// Layout 2 : UV/TexCoords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// Layout 3 : Tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	//Bind Index list
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 100 * 1024 * 1024, nullptr, GL_STATIC_DRAW);

	glBindVertexArray(0);

	//IndirectBuffer
	glGenBuffers(1, &indirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);


	//Instanced SSBO
	size_t instanceBufferSize = MAX_INSTANCES * sizeof(InstanceData);

	glGenBuffers(1, &instanceSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, instanceBufferSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, INSTANCE_BUFFER_BINDING_POINT, instanceSSBO, 0, instanceBufferSize);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INSTANCE_BUFFER_BINDING_POINT, instanceSSBO);


	////Shadow
	//ShadowIndirectBuffer
	glGenBuffers(1, &shadowIndirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, shadowIndirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	////Shadow
	//PointShadowIndirectBuffer
	glGenBuffers(1, &pointShadowIndirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, pointShadowIndirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	//Instanced SSBO - Shadow
	size_t shadowInstanceBufferSize = MAX_SHADOW_CASTER * sizeof(ShadowInstanceData);

	glGenBuffers(1, &shadowInstanceSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadowInstanceSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, shadowInstanceBufferSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, SHADOW_INSTANCE_BINDING_POINT, shadowInstanceSSBO, 0, shadowInstanceBufferSize);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADOW_INSTANCE_BINDING_POINT, shadowInstanceSSBO);
}

MeshCorrespondence* OpenGL_Renderer::AddMeshToRender(Component::MeshHandle newMesh) {
	// Check if the Mesh is already load
	uint32_t currentUniqueIndex = newMesh.meshPtr->Get_UniqueIndex();

	auto it = mesh_In_VertexList.find(currentUniqueIndex);
	if (it != mesh_In_VertexList.end()) {
		return &it->second;
	}
	Mesh currentMesh = *newMesh.meshPtr;

	MeshCorrespondence meshCorrespondence;
	meshCorrespondence.meshPtr = newMesh.meshPtr; //Weak Ptr

	//Build the correspondence for all of his subMesh
	for (size_t subMeshIndex = 0; subMeshIndex < currentMesh.subMeshs.size(); ++subMeshIndex) {
		SubMesh subMesh = static_cast<SubMesh>(std::get<OpenGL_SubMesh>(currentMesh.subMeshs[subMeshIndex]));

		//Create new a subMesh correspondence
		SubMeshCorrespondence subMesh_Correspondence;

		subMesh_Correspondence.vertexStart = vertexList.size();
		subMesh_Correspondence.vertexCount = subMesh.vertices.size();

		subMesh_Correspondence.indexStart = indexList.size();
		subMesh_Correspondence.indexCount = subMesh.indices.size();

		// Insertion into global buffers
		vertexList.insert(vertexList.end(), subMesh.vertices.begin(), subMesh.vertices.end());
		indexList.insert(indexList.end(), subMesh.indices.begin(), subMesh.indices.end());

		meshCorrespondence.subMesh_Correspondence.push_back(std::move(subMesh_Correspondence));
	}

	// Update buffer
	glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexList.size() * sizeof(Vertex), vertexList.data());
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexList.size() * sizeof(uint32_t), indexList.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	assert(vertexList.size() * sizeof(Vertex) <= 100 * 1024 * 1024 && "VBO overflow !");
	assert(indexList.size() * sizeof(uint32_t) <= 100 * 1024 * 1024 && "EBO overflow !");

	// Add to history
	mesh_In_VertexList[currentUniqueIndex] = std::move(meshCorrespondence);
	return &mesh_In_VertexList[currentUniqueIndex];
}

void OpenGL_Renderer::OrderDraw(Component::MeshHandle meshHandle, glm::mat4 modelMatrix) {
	MeshCorrespondence* correspondance = AddMeshToRender(meshHandle);

	recordedMeshInstances[correspondance].push_back(modelMatrix);
}

void OpenGL_Renderer::OrderShadowDraw(Component::MeshHandle meshHandle, glm::mat4 modelMatrix) {
	MeshCorrespondence* correspondance = AddMeshToRender(meshHandle);

	recordedMeshInstances_ShadowPass[correspondance].push_back(modelMatrix);
}

void OpenGL_Renderer::ExecuteRenderCommands() {
	if (recordedMeshInstances.empty()) return;

	//Temporal list
	std::vector<DrawElementsIndirectCommand> indirectCommands;
	std::vector<InstanceData> globalInstanceData;
	indirectCommands.reserve(recordedMeshInstances.size());

	// Iterate over all draw order of this form : (MeshCorrespondence, modelMatrix), previously add by OrderDraw
	for (auto& [mesh_Correspondence, matrices] : recordedMeshInstances) {
		if (mesh_Correspondence->subMesh_Correspondence.size() <= 0) continue;

		std::shared_ptr<Mesh> meshPtr = mesh_Correspondence->meshPtr.lock();
		Mesh currentMesh = *meshPtr;

		//Assert
		assert(mesh_Correspondence != nullptr && "Current Mesh_Correspondence is empty in ExecuteRenderCommands");
		assert(meshPtr && "NullPtr in execute commands");
		assert(mesh_In_VertexList.find(meshPtr->Get_UniqueIndex()) != mesh_In_VertexList.end() && "Current MeshCorrespondence don't exist in the global Vertex");
		assert(currentMesh.subMeshs.size() == mesh_Correspondence->subMesh_Correspondence.size() && "SubMesh and correspondence submesh are de-aligned");


		uint32_t instanceCount = static_cast<uint32_t>(matrices.size());

		// Iterate over all subMesh
		for (size_t subMeshIndex = 0; subMeshIndex < currentMesh.subMeshs.size(); ++subMeshIndex) {
			const SubMeshCorrespondence& corr = mesh_Correspondence->subMesh_Correspondence[subMeshIndex];


			OpenGL_SubMesh& subMesh = static_cast<OpenGL_SubMesh&>(std::get<OpenGL_SubMesh>(currentMesh.subMeshs[subMeshIndex]));
			Material* currentMat = assetStore->Get_Material(subMesh.material_Handle);



			// Retrieving bindless handles (with a fallback to the default texture if null)
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


	// Sending Matrices in the Instances SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, globalInstanceData.size() * sizeof(InstanceData), globalInstanceData.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INSTANCE_BUFFER_BINDING_POINT, instanceSSBO);

	// Sending Orders to the Indirect Buffer
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, indirectCommands.size() * sizeof(DrawElementsIndirectCommand), indirectCommands.data(), GL_DYNAMIC_DRAW);

	// Render
	glBindVertexArray(globalVAO);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(indirectCommands.size()), 0);

	glBindVertexArray(0);
}

// This is a lighter version of ExecuteRenderCommands for shadows,
// He doesn't call glMultiDrawElementsIndirect
// To start rendering, call ExecuteRenderCommands_Shadow
void OpenGL_Renderer::BuildInstance_ShadowSSBO() {
	if (recordedMeshInstances_ShadowPass.empty()) return;
	shadow_IndirectCommands.clear();
	pointShadow_IndirectCommands.clear();

	std::vector<ShadowInstanceData> global_Shadow_InstanceData;
	
	// Add by OrderShadowDraw
	for (auto& [mesh_Correspondence, matrices] : recordedMeshInstances_ShadowPass) {
		std::shared_ptr<Mesh> meshPtr = mesh_Correspondence->meshPtr.lock();
		Mesh currentMesh = *meshPtr;

		//Assert
		assert(mesh_Correspondence != nullptr && "Current Mesh_Correspondence is empty in BuildInstance_ShadowSSBO");
		assert(meshPtr && "NullPtr in BuildInstance_ShadowSSBO");
		assert(mesh_In_VertexList.find(meshPtr->Get_UniqueIndex()) != mesh_In_VertexList.end() && "Current MeshCorrespondence don't exist in the global Vertex in BuildInstance_ShadowSSBO");
		assert(currentMesh.subMeshs.size() == mesh_Correspondence->subMesh_Correspondence.size() && "SubMesh and correspondence submesh are de-aligned in BuildInstance_ShadowSSBO");

		uint32_t instanceCount = static_cast<uint32_t>(matrices.size());

		// Iterate over all subMesh
		for (size_t subMeshIndex = 0; subMeshIndex < currentMesh.subMeshs.size(); ++subMeshIndex) {
			const SubMeshCorrespondence& corr = mesh_Correspondence->subMesh_Correspondence[subMeshIndex];


			OpenGL_SubMesh& subMesh = static_cast<OpenGL_SubMesh&>(std::get<OpenGL_SubMesh>(currentMesh.subMeshs[subMeshIndex]));
			Material* currentMat = assetStore->Get_Material(subMesh.material_Handle);


			// Retrieving bindless handles (with a fallback to the default texture if null)
			Texture* diff = assetStore->Get_Texture(currentMat->diffuse_Text_Handle);
			uint64_t diffH = diff ? diff->bindlessHandle : Shader::GetDefaultText_Handle();



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
			openGLCmd.baseInstance = static_cast<uint32_t>(global_Shadow_InstanceData.size());

			shadow_IndirectCommands.push_back(openGLCmd);

			DrawElementsIndirectCommand pointCmd = openGLCmd;
			pointCmd.instanceCount = instanceCount * 6;
			pointShadow_IndirectCommands.push_back(pointCmd);


			// Apply the modelMatrix and create a ShadowInstanceData for each matrices/Submesh instance
			for (const auto& modelMatrix : matrices) {
				ShadowInstanceData s_instData;
				s_instData.modelMatrix = modelMatrix * localTransform;
				s_instData.diffuseTexHandle = diffH;

				global_Shadow_InstanceData.push_back(s_instData);
			}
		}
	}

	// Sending Matrices in the Shadow Instances SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadowInstanceSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, global_Shadow_InstanceData.size() * sizeof(ShadowInstanceData), global_Shadow_InstanceData.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADOW_INSTANCE_BINDING_POINT, shadowInstanceSSBO);

	// Sending Orders to the Indirect Buffer
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, shadowIndirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, shadow_IndirectCommands.size() * sizeof(DrawElementsIndirectCommand), shadow_IndirectCommands.data(), GL_DYNAMIC_DRAW);


	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, pointShadowIndirectBuffer); // nouveau buffer GL dédié
	glBufferData(GL_DRAW_INDIRECT_BUFFER, pointShadow_IndirectCommands.size() * sizeof(DrawElementsIndirectCommand), pointShadow_IndirectCommands.data(), GL_DYNAMIC_DRAW);
}

void OpenGL_Renderer::ExecuteRenderCommands_Shadow() {
	glBindVertexArray(globalVAO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, shadowIndirectBuffer);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(shadow_IndirectCommands.size()), 0);
	glBindVertexArray(0);
}
void OpenGL_Renderer::ExecuteRenderCommands_PointShadow() {
	glBindVertexArray(globalVAO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, pointShadowIndirectBuffer);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(pointShadow_IndirectCommands.size()), 0);
	glBindVertexArray(0);
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

// StartPos start at the upper-left corner 
void OpenGL_Renderer::SetViewport(glm::vec2 startPos, glm::vec2 newSize) {
	if (newSize.x < 0 || newSize.y < 0 || startPos.x < 0 || startPos.y < 0 ) return;
	glViewport(startPos.x, startPos.y, newSize.x, newSize.y);
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