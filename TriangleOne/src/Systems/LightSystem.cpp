  #include "Systems/LightSystem.h"

namespace Resource = Engine::Resource;
namespace Component = Engine::Component;
namespace Systems = Engine::Systems;

#pragma region Init shadow buffer 

void Systems::LightSystem::InitShadowMap(Component::DirLight* currentLight) {
	glGenFramebuffers(1, &currentLight->depthMapFBO);
	
	glGenTextures(1, &currentLight->depthMap);
	glBindTexture(GL_TEXTURE_2D, currentLight->depthMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, currentLight->SHADOW_WIDTH, currentLight->SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, currentLight->depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, currentLight->depthMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Shadow Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Systems::LightSystem::InitCubeMap(Component::PointLight* currentLight) {

	glGenFramebuffers(1, &currentLight->depthCubeMapFBO);

	glGenTextures(1, &currentLight->depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, currentLight->depthCubeMap);
	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, currentLight->SHADOW_WIDTH, currentLight->SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glBindFramebuffer(GL_FRAMEBUFFER, currentLight->depthCubeMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, currentLight->depthCubeMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "CubeMap Shadow Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Systems::LightSystem::InitSpotShadowMap(Component::SpotLight* currentLight) {

	glGenFramebuffers(1, &currentLight->depthMapFBO);

	glGenTextures(1, &currentLight->depthMap);
	glBindTexture(GL_TEXTURE_2D, currentLight->depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, currentLight->SHADOW_WIDTH, currentLight->SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, currentLight->depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, currentLight->depthMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Shadow Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Systems::LightSystem::InitShadowBuffer(World& world) {
	std::vector<int> to_remove;
	View view = world.view<Component::LightToInitTag>();
	view.each([&](int entity, Component::LightToInitTag& lightTag) {
		switch (lightTag.tag)
		{
		case Component::LightTag::None:
			assert(false && "Init Tag not define on a light");
			break;
		case Component::LightTag::PointLight_Tag: {
			Component::PointLight* currentLight = world.get_component<Component::PointLight>(entity);
			if (currentLight != nullptr) {
				InitCubeMap(currentLight);
				to_remove.push_back(entity);
				break;
			}
			[[fallthrough]];
		}
		case Component::LightTag::SpotLight_Tag: {
			Component::SpotLight* currentLight = world.get_component<Component::SpotLight>(entity);
			if (currentLight != nullptr) {
				InitSpotShadowMap(currentLight);
				to_remove.push_back(entity);
				break;
			}
			[[fallthrough]];
		}
		case Component::LightTag::Directional_Tag: {
			Component::DirLight* currentLight = world.get_component<Component::DirLight>(entity);
			if (currentLight != nullptr) {
				InitShadowMap(currentLight);
				to_remove.push_back(entity);
				break;
			}
			[[fallthrough]];
		}
		default:
			assert(true && "Unexpected Error in InitShadowBuffer");
			break;
		}
	});
	for (int entity : to_remove) {
		world.remove_component<Component::LightToInitTag>(entity);
	}
}

#pragma endregion 

#pragma region Shadow

void Systems::LightSystem::DrawShadowForDirLight(World* world, Resource::RenderResource& renderResource, All_Light& lights) {  // Bug sur la window si resize
	world->renderer->SetViewport_Size(glm::vec2(lights.dirLight_Shadow_Size.first, lights.dirLight_Shadow_Size.second));
	glBindFramebuffer(GL_FRAMEBUFFER, lights.dirLight_DepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	Shader* depthShader = renderResource.r_Shader.depthShader.get();

	depthShader->Use();
	depthShader->setMatrix("lightSpaceMatrix", lights.dirLight_Matrice);

	View view = world->view<Component::MeshHandle, Component::SceneTag, Component::Transform>();
	view.each([&](int entity, Component::MeshHandle& meshHandle, Component::SceneTag& sceneTag, Component::Transform& transform) {
		if (meshHandle.index != -1  && meshHandle.haveToBeDraw && meshHandle.castShadow && sceneTag.scene_id == 0) {
			Mesh currentMesh = world->assetStore->Get_Mesh(meshHandle.index);

			depthShader->setMatrix("model", transform.GetTransformModel());
			world->renderer->DrawMesh_Without_Texture(currentMesh);
		}

	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	world->renderer->SetViewport_Size(glm::vec2(renderResource.renderWIDTH, renderResource.renderHEIGHT));
}

void Systems::LightSystem::DrawShadowForPointLight(World* world, Resource::RenderResource& renderResource, All_Light& lights, int index) {
	std::pair<unsigned int, unsigned int> shadowSize = lights.pointLights_Shadow_Size[index];
	glm::vec3 position = lights.pointLights[index].position;
	Shader* depthShader = renderResource.r_Shader.depthShaderCubeMap.get();
	float range = lights.pointLights[index].range;

	float aspect = (float)shadowSize.first / (float)shadowSize.second;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, 0.1f, range);


	world->renderer->SetViewport_Size(glm::vec2(shadowSize.first, shadowSize.second));
	glBindFramebuffer(GL_FRAMEBUFFER, lights.pointLights_DepthMapFBO[index]);  // Fbo unique par point light
	glClear(GL_DEPTH_BUFFER_BIT);


	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));


	depthShader->Use();
	depthShader->setFloat("far_plane", range);
	depthShader->setVec("lightPos", position);


	for (int i = 0; i < shadowTransforms.size(); i++) {
		depthShader->setMatrix("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	}


	View view = world->view<Component::MeshHandle, Component::SceneTag, Component::Transform>();
	view.each([&](int entity, Component::MeshHandle& meshHandle, Component::SceneTag& sceneTag, Component::Transform& transform) {
		if (meshHandle.index != -1  && meshHandle.haveToBeDraw && meshHandle.castShadow && sceneTag.scene_id == 0) {
			Mesh currentMesh = world->assetStore->Get_Mesh(meshHandle.index);

			depthShader->setMatrix("model", transform.GetTransformModel());
			world->renderer->DrawMesh_Without_Texture(currentMesh);
		}
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	world->renderer->SetViewport_Size(glm::vec2(renderResource.renderWIDTH, renderResource.renderHEIGHT));
}

void Systems::LightSystem::DrawShadowForSpotLight(World* world, Resource::RenderResource& renderResource, All_Light& lights, int index) {
	std::pair<unsigned int, unsigned int> shadowSize = lights.spotLights_Shadow_Size[index];
	glm::vec3 position = lights.spotLights[index].position;
	glm::vec3 direction = lights.spotLights[index].direction;
	Shader* depthShader = renderResource.r_Shader.depthShader.get();


	float aspect = (float)shadowSize.first / (float)shadowSize.second;
	glm::mat4 shadowProj = glm::perspective(glm::radians(lights.spotLights[index].outerCutOff * 2.0f), aspect, 0.1f, lights.spotLights[index].range);

	glm::vec3 up = (glm::abs(direction.y) > 0.99f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
	glm::mat4 shadowView = glm::lookAt(position, position + direction, up);


	glm::mat4 lightSpaceMatrix = shadowProj * shadowView;

	world->renderer->SetViewport_Size(glm::vec2(shadowSize.first, shadowSize.second));
	glBindFramebuffer(GL_FRAMEBUFFER, lights.spotLights_DepthMapFBO[index]);  // Fbo unique par spot light
	glClear(GL_DEPTH_BUFFER_BIT );


	depthShader->Use();
	depthShader->setMatrix("lightSpaceMatrix", lightSpaceMatrix);


	View view = world->view<Component::MeshHandle, Component::SceneTag, Component::Transform>();
	view.each([&](int entity, Component::MeshHandle& meshHandle, Component::SceneTag& sceneTag, Component::Transform& transform) {
		if (meshHandle.index != -1 && meshHandle.haveToBeDraw && meshHandle.castShadow && sceneTag.scene_id == 0) {
			Mesh currentMesh = world->assetStore->Get_Mesh(meshHandle.index);

			depthShader->setMatrix("model", transform.GetTransformModel());
			world->renderer->DrawMesh_Without_Texture(currentMesh);
		}
	});


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	world->renderer->SetViewport_Size(glm::vec2(renderResource.renderWIDTH, renderResource.renderHEIGHT));
}

void Systems::LightSystem::ShadowPass(World* world, Resource::RenderResource* renderResource, All_Light* lights) {
	glCullFace(GL_BACK);
	DrawShadowForDirLight(world, *renderResource, *lights);

	for (int i = 0; i < lights->pointLights.size(); i++) {
		DrawShadowForPointLight(world, *renderResource, *lights, i);
	}

	for (int i = 0; i < lights->spotLights.size(); i++) {
		DrawShadowForSpotLight(world, *renderResource, *lights, i);
	}
}

void Systems::LightSystem::SendDepthMapToLightningShader(World* world, const Resource::RenderResource* renderResource, const Resource::ResourceBuffer* resourceBuffer, All_Light* lights) {
	if (lights->pointLights_DepthMap.size() >= MAX_POINT_LIGHT) std::cout << "Max pointLight number reach" << std::endl;
	if (lights->spotLights_DepthMap.size() >= MAX_SPOT_LIGHT) std::cout << "Max spotLight number reach" << std::endl;


	Shader* currentShader = renderResource->r_Shader.lightningPass_Shader.get();
	currentShader->Use();
	// --- TEXTURE UNIT MANAGEMENT ---

	const int SLOT_SHADOW_DIR = 16;
	const int SLOT_SHADOW_POINT_START = 17;
	const int SLOT_SHADOW_SPOT_START = 25;

	// --- 2. GESTION LUMIERE DIRECTIONNELLE (Shadow Map 2D) ---
	currentShader->setInt("shadowMap", SLOT_SHADOW_DIR);

	glActiveTexture(GL_TEXTURE0 + SLOT_SHADOW_DIR);
	if (lights->dirLight_DepthMap != 0) {  // WARNING, do a better check if the sun existe
		glBindTexture(GL_TEXTURE_2D, lights->dirLight_DepthMap);
		currentShader->setMatrix("lightSpaceMatrix", lights->dirLight_Matrice);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, resourceBuffer->renderResource->dummyDepthMap2D);
	}

	// --- 3. GESTION POINT LIGHTS (Shadow Cube Maps) ---
	int maxPointLights = 8;
	int activeLights = std::min((int)lights->pointLights_DepthMap.size(), maxPointLights);


	//Component::PointLight
	for (int i = 0; i < maxPointLights; i++) {

		// Construction du nom "shadowCubeMaps[0]", "shadowCubeMaps[1]"...
		std::string uniformName = "shadowCubeMaps[" + std::to_string(i) + "]";

		int currentSlot = SLOT_SHADOW_POINT_START + i;

		// 1. On dit au shader : "Le sampler i doit lire dans le slot X"
		currentShader->setInt(uniformName, currentSlot);

		// 2. On active le slot X
		glActiveTexture(GL_TEXTURE0 + currentSlot);

		if (i < activeLights) {
			glBindTexture(GL_TEXTURE_CUBE_MAP, lights->pointLights_DepthMap[i]);
		}
		else {
			// Nettoyage des slots inutilisés (évite les bugs de "Sampler Type Mismatch")
			glBindTexture(GL_TEXTURE_CUBE_MAP, resourceBuffer->renderResource->dummyDepthCubeMap);
		}
	}

	//// Gestion Spot Light
	int maxSpotLights = 8;
	int activeSpotLights = std::min((int)lights->spotLights_DepthMap.size(), maxSpotLights);
	for (int i = 0; i < maxSpotLights; i++) {
		std::string uniformName = "shadowMapSpot[" + std::to_string(i) + "]";
		std::string uniformNameMatrice = "spotLightMatrices[" + std::to_string(i) + "]";

		int currentSlot = SLOT_SHADOW_SPOT_START + i;

		currentShader->setInt(uniformName, currentSlot);


		glActiveTexture(GL_TEXTURE0 + currentSlot);


		if (i < activeSpotLights) {
			currentShader->setMatrix(uniformNameMatrice, lights->spotLight_Matrice[i]);
			glBindTexture(GL_TEXTURE_2D, lights->spotLights_DepthMap[i]);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, resourceBuffer->renderResource->dummyDepthMap2D);
		}
	}

	glActiveTexture(GL_TEXTURE0);

	// --- END TEXTURE MANAGEMENT ---
}

#pragma endregion

#pragma region Light

#pragma region Init
void Systems::LightSystem::InitLightSSBO(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	int uniform_Light_Binding_Point = 1;
	Resource::RenderResource* renderResource = resourceBuffer->renderResource;

	glGenBuffers(1, &renderResource->light_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderResource->light_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Padding_DirLight) + MAX_POINT_LIGHT * sizeof(Padding_PointLight) + MAX_SPOT_LIGHT * sizeof(Padding_SpotLight) + sizeof(int) * 2, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	// BINDING SSBO slot 1, light
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, uniform_Light_Binding_Point, renderResource->light_SSBO, 0, sizeof(Padding_DirLight) + MAX_POINT_LIGHT * sizeof(Padding_PointLight) + MAX_SPOT_LIGHT * sizeof(Padding_SpotLight) + sizeof(int) * 2);

	renderResource->lightSSBO_Data_Size.push_back(sizeof(Padding_DirLight));
	renderResource->lightSSBO_Data_Size.push_back(MAX_POINT_LIGHT * sizeof(Padding_PointLight));
	renderResource->lightSSBO_Data_Size.push_back(MAX_SPOT_LIGHT * sizeof(Padding_SpotLight));
}

void Systems::LightSystem::Init_IrradianceMap(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::RenderResource* renderData = resourceBuffer->renderResource;

	glGenTextures(1, &renderData->irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->irradianceMap);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 64, 64, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Systems::LightSystem::InitCaptureCubeMap(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::RenderResource* renderData = resourceBuffer->renderResource;

	glGenFramebuffers(1, &renderData->captureFBO);
	glGenRenderbuffers(1, &renderData->captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, renderData->captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, renderData->captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderData->captureRBO);

	glGenTextures(1, &renderData->envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Capture Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Systems::LightSystem::InitPrefilter_IBL(World& world, Resource::RenderResource* renderData) {
	glGenTextures(1, &renderData->prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->prefilterMap);

	glTexStorage2D(GL_TEXTURE_CUBE_MAP, renderData->maxMipLevels, GL_RGB16F, renderData->specularResolution_Map, renderData->specularResolution_Map);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Systems::LightSystem::Init_BRDF_LUTTexture(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::RenderResource* renderData = resourceBuffer->renderResource;
	glGenTextures(1, &renderData->brdfLUTTexture);

	glBindTexture(GL_TEXTURE_2D, renderData->brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

#pragma endregion

void Systems::LightSystem::ConvulateEnvCube(World& world, const Resource::ResourceBuffer* resourceBuffer, glm::mat4 captureProjection, glm::mat4 captureViews[]) {
	Resource::RenderResource* renderData = resourceBuffer->renderResource;
	Resource::WindowResource* windowData = resourceBuffer->windowResource;
	Shader* shader = renderData->r_Shader.irradiance_Shader.get();

	glBindFramebuffer(GL_FRAMEBUFFER, renderData->captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, renderData->captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 64, 64);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderData->captureRBO);

	shader->Use();
	glActiveTexture(GL_TEXTURE0);
	shader->setInt("environmentMap", 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->envCubemap);
	shader->setMatrix("projection", captureProjection);
	world.renderer->SetViewport_Size(glm::vec2(64, 64));


	for (unsigned int i = 0; i < 6; ++i)
	{
		shader->setMatrix("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, renderData->irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		world.renderer->DrawCube();
	}
	world.renderer->SetViewport_Size(glm::vec2(renderData->renderWIDTH, renderData->renderHEIGHT));


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Systems::LightSystem::Prefilter_EnvCub(World& world, Resource::RenderResource* renderData, glm::mat4 captureProjection, glm::mat4 captureViews[]) {
	InitPrefilter_IBL(world, renderData);

	Shader* prefilter_Shader = renderData->r_Shader.prefilter_Shader.get();

	prefilter_Shader->Use();
	prefilter_Shader->setMatrix("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->envCubemap);
	prefilter_Shader->setInt("environmentMap", 0);

	glBindFramebuffer(GL_FRAMEBUFFER, renderData->captureFBO);


	for (unsigned int mip = 0; mip < renderData->maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = renderData->specularResolution_Map * std::pow(0.5, mip);
		unsigned int mipHeight = renderData->specularResolution_Map * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, renderData->captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);

		world.renderer->SetViewport_Size(glm::vec2(mipWidth, mipHeight));
		float roughness = (float)mip / (float)(renderData->maxMipLevels - 1);
		prefilter_Shader->setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilter_Shader->setMatrix("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, renderData->prefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			world.renderer->DrawCube();
		}
	}
	world.renderer->SetViewport_Size(glm::vec2(renderData->renderWIDTH, renderData->renderHEIGHT));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Systems::LightSystem::Equirenctangular_To_CubeMap(World& world, const Resource::ResourceBuffer* resourceBuffer, std::string equirectangularMap_Path) {
	Resource::RenderResource* renderData = resourceBuffer->renderResource;
	Resource::WindowResource* windowData = resourceBuffer->windowResource;

	glBindFramebuffer(GL_FRAMEBUFFER, renderData->captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, renderData->captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderData->captureRBO);
	world.renderer->SetViewport_Size(glm::vec2(512, 512));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO not complete before equirect render!" << std::endl;

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	};

	Shader* equirectangular_To_CubemapShader = renderData->r_Shader.equirectangular_To_CubemapShader.get();

	unsigned int equiRec_Map = TextureClass::LoadEquirectangularTex(equirectangularMap_Path);
	equirectangular_To_CubemapShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, equiRec_Map);
	equirectangular_To_CubemapShader->setInt("equirectangularMap", 0);
	equirectangular_To_CubemapShader->setMatrix("projection", captureProjection);



	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangular_To_CubemapShader->setMatrix("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, renderData->envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		world.renderer->DrawCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	world.renderer->SetViewport_Size(glm::vec2(renderData->renderWIDTH, renderData->renderHEIGHT));
	glDeleteTextures(1, &equiRec_Map);

	glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	ConvulateEnvCube(world, resourceBuffer, captureProjection, captureViews);
	Prefilter_EnvCub(world, renderData, captureProjection, captureViews);

	//////brdfLUt
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, renderData->captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderData->brdfLUTTexture, 0);

	renderData->r_Shader.brdf_Shader->Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	world.renderer->DrawQuad(renderData);
	//////


	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

glm::vec3 Systems::LightSystem::Calc_SpotLightDirection(glm::mat4 transformModel, glm::vec3 lightDirection) {
	return glm::normalize(glm::vec3(transformModel * glm::vec4(lightDirection, 0.0f)));
}

void Systems::LightSystem::UpdateLight(World* world, Resource::RenderResource* renderResource, All_Light& lights) {
	for (auto& spotLight : lights.spotLights) {  // This value need to be calculate at the last minute
		spotLight.cutOff = glm::cos(glm::radians(spotLight.cutOff));
		spotLight.outerCutOff = glm::cos(glm::radians(spotLight.outerCutOff));
	}

	size_t offset_End_DirLight = renderResource->lightSSBO_Data_Size[0];
	size_t offset_End_PointLight = offset_End_DirLight + renderResource->lightSSBO_Data_Size[1];
	size_t offset_End_SpotLight = offset_End_PointLight + renderResource->lightSSBO_Data_Size[2];

	int activePointLights = std::min((int)lights.pointLights.size(), MAX_POINT_LIGHT); // Bloquer à 8 max
	int activeSpotLights = std::min((int)lights.spotLights.size(), MAX_SPOT_LIGHT); // Bloquer à 8 max

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderResource->light_SSBO);


	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, offset_End_DirLight, &lights.dirLight); // Component::DirLight
	if (!lights.pointLights.empty()) {
		size_t actual_PointLights_Size = lights.pointLights.size() * sizeof(Padding_PointLight);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_DirLight, actual_PointLights_Size, lights.pointLights.data()); // Component::PointLight
	}
	if (!lights.spotLights.empty()) {
		size_t actual_SpotLights_Size = lights.spotLights.size() * sizeof(Padding_SpotLight);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_PointLight, actual_SpotLights_Size, lights.spotLights.data()); // Component::SpotLight
	}

	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_SpotLight, sizeof(int), &activePointLights);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_SpotLight + sizeof(int), sizeof(int), &activeSpotLights);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

All_Light* Systems::LightSystem::DataCollector(World* world, Component::CameraComponent* mainCamera, Resource::RenderResource* renderRessource) {
	All_Light* lights = new All_Light();
	View viewDirLight = world->view<Component::DirLight>();

	int starCompteur = 0;

	viewDirLight.each([&](int entity, Component::DirLight& dirLight) {
		Padding_DirLight p_DirLight;

		starCompteur++;
		if (starCompteur <= 1) {

			//Dir_Light
			p_DirLight.color = dirLight.color * dirLight.intensity;
			p_DirLight.direction = dirLight.direction;
			lights->dirLight = p_DirLight;

			//Shadow
			lights->dirLight_DepthMap = dirLight.depthMap;
			lights->dirLight_DepthMapFBO = dirLight.depthMapFBO;
			lights->dirLight_Shadow_Size = std::make_pair(dirLight.SHADOW_WIDTH, dirLight.SHADOW_HEIGHT);

			//Matrices
			lights->dirLight_Matrice = dirLight.UpdateMatrix(mainCamera->viewMatrice, renderRessource->projection);
		}
		else {
			std::cout << "Multiple dir light detected, only the first one will be take into consideration" << std::endl;
		}

		});
	int pointLight_compteur = 0;
	View viewPointLight = world->view<Component::PointLight, Component::Transform>();
	viewPointLight.each([&](int entity, Component::PointLight& pointLight, Component::Transform& transform) {
		pointLight_compteur++;
		if (pointLight_compteur <= MAX_POINT_LIGHT) {
			Padding_PointLight p_pointLight;
			//Point_Light
			p_pointLight.position = transform.position;
			p_pointLight.color = pointLight.color * pointLight.intensity;
			p_pointLight.range = pointLight.range;
			lights->pointLights.push_back(p_pointLight);

			//Shadow
			lights->pointLights_DepthMap.push_back(pointLight.depthCubeMap);
			lights->pointLights_DepthMapFBO.push_back(pointLight.depthCubeMapFBO);
			lights->pointLights_Shadow_Size.push_back(std::make_pair(pointLight.SHADOW_WIDTH, pointLight.SHADOW_HEIGHT));
		}
		else {
			std::cout << "Component::PointLight number exceed the limit of: " << MAX_POINT_LIGHT << " Some light will not be taking into consideration, current nbr pointLight: " << pointLight_compteur << std::endl;
		}

		});
	int spotLight_compteur = 0;
	View viewSpotLight = world->view<Component::SpotLight, Component::Transform>();
	viewSpotLight.each([&](int entity, Component::SpotLight& spotLight, Component::Transform& transform) {
		spotLight_compteur++;
		if (spotLight_compteur <= MAX_SPOT_LIGHT) {
			Padding_SpotLight p_spotLight;
			//Spot_Light
			p_spotLight.position = transform.position;
			p_spotLight.direction = Calc_SpotLightDirection(transform.GetTransformModel(), spotLight.direction);
			p_spotLight.color = spotLight.color * spotLight.intensity;
			p_spotLight.range = spotLight.range;
			p_spotLight.cutOff = spotLight.cutOff;
			p_spotLight.outerCutOff = spotLight.outerCutOff;

			lights->spotLights.push_back(p_spotLight);

			//Shadow
			lights->spotLights_DepthMap.push_back(spotLight.depthMap);
			lights->spotLights_DepthMapFBO.push_back(spotLight.depthMapFBO);
			lights->spotLights_Shadow_Size.push_back(std::make_pair(spotLight.SHADOW_WIDTH, spotLight.SHADOW_HEIGHT));

			//Matrices
			spotLight.aspect = (float)spotLight.SHADOW_WIDTH / (float)spotLight.SHADOW_HEIGHT;
			glm::mat4 projection = glm::perspective(glm::radians(p_spotLight.outerCutOff * 2.0f), spotLight.aspect, 0.1f, p_spotLight.range);

			glm::vec3 up = (glm::abs(p_spotLight.direction.y) > 0.99f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
			glm::mat4 lightView = glm::lookAt(p_spotLight.position, p_spotLight.position + p_spotLight.direction, up);


			spotLight.lightSpaceMatrix = projection * lightView;

			lights->spotLight_Matrice.push_back(spotLight.lightSpaceMatrix);
		}
		else {
			std::cout << "Component::SpotLight number exceed the limit of: " << MAX_SPOT_LIGHT << " Some light will not be taking into consideration, current nbr spotLight: " << spotLight_compteur << std::endl;
		}

		});

	return lights;
}

#pragma endregion

#pragma region Draw
void Systems::LightSystem::Draw_SkyBox(World* world, const Resource::ResourceBuffer* resourceBuffer, glm::mat4 viewMatrice) {
	Resource::RenderResource* renderData = resourceBuffer->renderResource;
	Resource::WindowResource* windowResource = resourceBuffer->windowResource;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, renderData->gBuffer.gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderData->framebuffer);

	glBlitFramebuffer(0, 0, windowResource->WIDTH, windowResource->HEIGHT, 0, 0, windowResource->WIDTH, windowResource->HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	Shader* skyBoxshader = renderData->r_Shader.skyBox_Shader.get();
	skyBoxshader->Use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->envCubemap);
	skyBoxshader->setInt("environmentMap", 0);

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	skyBoxshader->setMatrix("rootView", glm::mat4(glm::mat3(viewMatrice)));
	skyBoxshader->setMatrix("projection", renderData->projection);
	world->renderer->DrawCube();

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}

void Systems::LightSystem::Draw_BloomBlurEffect(Resource::RenderResource* renderData) {
	renderData->horizontal = true;
	bool first_iteration = true;
	unsigned int amount = renderData->bloom_iteration;

	Shader* bloomShader = renderData->r_Shader.bloomShader.get();

	bloomShader->Use();
	glActiveTexture(GL_TEXTURE0);
	bloomShader->setInt("image", 0);

	glBindVertexArray(renderData->quadVAO);
	glDisable(GL_DEPTH_TEST);
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderData->pingpongFBO[renderData->horizontal]);
		bloomShader->setBool("horizontal", renderData->horizontal);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? renderData->finalTxtColorOutput[1] : renderData->pingpongBuffers[!renderData->horizontal]);

		//Draw
		glDrawArrays(GL_TRIANGLES, 0, 6);

		renderData->horizontal = !renderData->horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//Post process
void Systems::LightSystem::Draw_FinalPass(Resource::RenderResource* renderData) {
	if (renderData->bloomEnable) {
		Draw_BloomBlurEffect(renderData);
	}

	Shader* postProcessShader = renderData->r_Shader.postProcessShader.get();

	//glBindFramebuffer(GL_FRAMEBUFFER, renderData->toImGui_FBO);
	//glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->toImGui_FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Post process
	postProcessShader->Use();
	glBindVertexArray(renderData->quadVAO);
	//Parameters
	postProcessShader->setFloat("exposure", renderData->exposure);
	glDisable(GL_DEPTH_TEST);

	//Scene image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderData->finalTxtColorOutput[0]);
	postProcessShader->setInt("scene", 0);

	//Bloom image
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, renderData->pingpongBuffers[!renderData->horizontal]); // Le dernier buffer utilisé
	postProcessShader->setInt("bloomBlur", 1);
	postProcessShader->setBool("bloomEnable", renderData->bloomEnable);

	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtColorOutput);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
}

//Lightning Pass
void Systems::LightSystem::LightningPass(World* world, Component::Transform* transformMainCamera, const Resource::ResourceBuffer* resourceBuffer, glm::mat4 viewMatrice) {
	Resource::InterfaceRessource* interfaceRessource = resourceBuffer->interfaceRessource;
	Resource::RenderResource* renderResource = resourceBuffer->renderResource;
	glBindFramebuffer(GL_FRAMEBUFFER, renderResource->framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader* lighningShader = renderResource->r_Shader.lightningPass_Shader.get();

	lighningShader->Use();
	lighningShader->setVec("viewPos", transformMainCamera->position);

	int i = 0;
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, renderResource->gBuffer.gPosition);
	lighningShader->setInt("gPosition", i++);
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, renderResource->gBuffer.gNormal);
	lighningShader->setInt("gNormal", i++);
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, renderResource->gBuffer.gAlbedo);
	lighningShader->setInt("gAlbedo", i++);
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, renderResource->gBuffer.gDepth);
	lighningShader->setInt("gDepth", i++);
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, renderResource->gBuffer.gARM);
	lighningShader->setInt("gARM", i++);
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, renderResource->ssao.ssaoBlurText);
	lighningShader->setInt("ssaoTexture", i++);
	lighningShader->setInt("renderTarget", interfaceRessource->renderTarget);

	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderResource->irradianceMap);
	lighningShader->setInt("irradianceMap", i++);
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderResource->prefilterMap);
	lighningShader->setInt("prefilterMap", i++);
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, renderResource->brdfLUTTexture);
	lighningShader->setInt("brdfLUT", i++);


	world->renderer->DrawQuad(renderResource);
	//Skybox
	Draw_SkyBox(world, resourceBuffer, viewMatrice);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#pragma endregion


void Systems::LightSystem::Init(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	static_assert(sizeof(Padding_DirLight) == 32, "Invalide alignement");
	static_assert(alignof(Padding_DirLight) == 16);

	static_assert(sizeof(Padding_PointLight) == 32, "Invalide alignement");
	static_assert(alignof(Padding_PointLight) == 16);

	static_assert(sizeof(Padding_SpotLight) == 64, "Invalide alignement");
	static_assert(alignof(Padding_SpotLight) == 16);

	resourceBuffer->renderResource->lights = new All_Light();

	InitLightSSBO(world, resourceBuffer);

	InitCaptureCubeMap(world, resourceBuffer);
	Init_IrradianceMap(world, resourceBuffer);
	Init_BRDF_LUTTexture(world, resourceBuffer);

	Equirenctangular_To_CubeMap(world, resourceBuffer, "Assets/SkyBox/inTheSky/kloofendal_48d_partly_cloudy_puresky_2k.hdr");  //qwantani_night_puresky_2k
}

void Systems::LightSystem::Update(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	InitShadowBuffer(world);

	Resource::RenderResource* renderResource = world.get_ressource<Resource::RenderResource>();
	Resource::WindowResource* windowResource = world.get_ressource<Resource::WindowResource>();
	if (windowResource->isIconified) return;

	/////////////////Camera
	Entity entityCam = resourceBuffer->activeCamera->cameraID;
	Component::CameraComponent* mainCamera = world.get_component<Component::CameraComponent>(entityCam);
	Component::Transform* transformMainCamera = world.get_component<Component::Transform>(entityCam);
	if (mainCamera == nullptr || transformMainCamera == nullptr) {  // Pas de main camera, pas de rendu
		std::cout << "Main camera have a null value" << std::endl;
		return;
	}
	/////////////////

	renderResource->lights = DataCollector(&world, mainCamera, renderResource);

	ShadowPass(&world, renderResource, renderResource->lights);
	UpdateLight(&world, renderResource, *renderResource->lights);
	SendDepthMapToLightningShader(&world, renderResource, resourceBuffer, renderResource->lights);
	glDisable(GL_CULL_FACE);
	LightningPass(&world, transformMainCamera, resourceBuffer, mainCamera->viewMatrice);
	Draw_FinalPass(renderResource);
	glEnable(GL_CULL_FACE);

	//glfwSwapBuffers(windowResource->window);
	glfwPollEvents();
	delete renderResource->lights;
}