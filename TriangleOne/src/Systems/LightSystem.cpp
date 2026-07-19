  #include "Systems/LightSystem.h"

namespace Resource = Engine::Resource;
namespace Component = Engine::Component;
namespace Systems = Engine::Systems;
namespace Render = Engine::Render;
namespace Physics = Engine::Physics; // To display jolt shape

#pragma region Init Shadow 

void Systems::LightSystem::InitShadowArray(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::ShadowResource* shadowData = resourceBuffer->shadowResource;

	const float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	int startQuality = 256;
	int currentQuality = startQuality;

	//PointLight
	glGenTextures(3, &shadowData->point_Array[0]);
	for (int i = 0; i < 3; i++) {
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowData->point_Array[i]);

		glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT24, currentQuality, currentQuality, MAX_SHADOW_CASTER_POINT_LIGHT * 6, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		glTexParameterfv(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

		//Bindless
		shadowData->bindless_PointArray[i] = glGetTextureHandleARB(shadowData->point_Array[i]);
		glMakeTextureHandleResidentARB(shadowData->bindless_PointArray[i]);

		currentQuality *= 2;
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

	currentQuality = startQuality;


	//SpotLight
	glGenTextures(3, &shadowData->spot_Array[0]);
	for (int i = 0; i < 3; i++) {
		glBindTexture(GL_TEXTURE_2D_ARRAY, shadowData->spot_Array[i]);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT24, currentQuality, currentQuality, MAX_SHADOW_CASTER_SPOT_LIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

		//Bindless
		shadowData->bindless_SpotArray[i] = glGetTextureHandleARB(shadowData->spot_Array[i]);
		glMakeTextureHandleResidentARB(shadowData->bindless_SpotArray[i]);

		currentQuality *= 2;
	}
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Systems::LightSystem::InitShadowSSBO(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::ShadowResource* shadowData = resourceBuffer->shadowResource;

	size_t dirLightSize = sizeof(Render::ShadowMap);
	size_t pointLightSize = sizeof(Render::ShadowMap_PointLight) * MAX_SHADOW_CASTER_POINT_LIGHT;
	size_t spotLightSize = sizeof(Render::ShadowMap) * MAX_SHADOW_CASTER_SPOT_LIGHT;

	glGenBuffers(1, &shadowData->SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadowData->SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, dirLightSize + pointLightSize + spotLightSize , NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, SHADOW_BINDING_POINT, shadowData->SSBO, 0, dirLightSize + pointLightSize + spotLightSize);
	//BINDING SSBO
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADOW_BINDING_POINT, shadowData->SSBO);

	shadowData->SSBO_Data_Size.push_back(sizeof(Render::ShadowMap));
	shadowData->SSBO_Data_Size.push_back(MAX_SHADOW_CASTER_POINT_LIGHT * sizeof(Render::ShadowMap_PointLight));
	shadowData->SSBO_Data_Size.push_back(MAX_SHADOW_CASTER_SPOT_LIGHT * sizeof(Render::ShadowMap));
}

void Systems::LightSystem::InitShadowText_SSBO(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::ShadowResource* shadowData = resourceBuffer->shadowResource;

	glGenBuffers(1, &shadowData->Text_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadowData->Text_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint64_t) * 6, NULL, GL_STATIC_READ);  // dirLight shadowMap + shadowArray(low, medium, hight, veryhight)
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, SHADOW_TEXT_BINDING_POINT, shadowData->Text_SSBO, 0, sizeof(uint64_t) * 6);
	//BINDING SSBO
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADOW_TEXT_BINDING_POINT, shadowData->Text_SSBO);

	//Binding text
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint64_t) * 3, &shadowData->bindless_PointArray);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(uint64_t) * 3, sizeof(uint64_t) * 3, &shadowData->bindless_SpotArray);

}

void Systems::LightSystem::InitShadowFBO(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::ShadowResource* shadowData = resourceBuffer->shadowResource;

	//PointLight
	glGenFramebuffers(3, &shadowData->point_FBO[0]);
	for (int i = 0; i < 3; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, shadowData->point_FBO[i]);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowData->point_Array[i], 0, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Shadow Framebuffer not complete!" << std::endl;
	}

	//SpotLight
	glGenFramebuffers(3, &shadowData->spot_FBO[0]);
	for (int i = 0; i < 3; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, shadowData->spot_FBO[i]);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowData->spot_Array[i], 0, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Shadow Framebuffer not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//Resize shadowText if textureQuality don't match with the expected quality
void static Resize_ShadowText(unsigned int shadowText, Render::Shadow_Quality& textureQuality, Render::Shadow_Quality expectedQuality) {
	if (textureQuality == expectedQuality) return;

	float newQuality = Render::ShadowQuality_To_Value(expectedQuality);
	glBindTexture(GL_TEXTURE_2D, shadowText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, newQuality, newQuality, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	textureQuality = expectedQuality;
}

//Create The dir light FBO
void Systems::LightSystem::InitDirLightFBO(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::ShadowResource* shadowData = resourceBuffer->shadowResource;

	glGenTextures(1, &shadowData->dirLight_depthMap);
	glBindTexture(GL_TEXTURE_2D, shadowData->dirLight_depthMap);

	float defaultQuality = Render::ShadowQuality_To_Value(shadowData->dirLight_Current_Quality);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, defaultQuality, defaultQuality, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


	glGenFramebuffers(1, &shadowData->dirLight_depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowData->dirLight_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowData->dirLight_depthMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Shadow Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#pragma endregion 

#pragma region Shadow

void Systems::LightSystem::ClearShadowsMap(World* world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::ShadowResource* shadowData = resourceBuffer->shadowResource;
	const float depth = 1.0f;

	//Clear shadow Array
	//PointLight
	for (int i = 0; i < 3; ++i){
		glClearTexImage(shadowData->point_Array[i], 0, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	}
	//SpotLight
	for (int i = 0; i < 3; ++i){
		glClearTexImage(shadowData->spot_Array[i], 0, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	}
}

void Systems::LightSystem::DrawShadow(World* world, const Resource::ResourceBuffer* resourceBuffer) {
	int last_Empty_Slot_Spot[4] = { 0,0,0,0 };
	int last_Empty_Slot_Point[4] = { 0,0,0,0 };

	Resource::ShadowResource* shadowData = resourceBuffer->shadowResource;
	Resource::RenderResource* renderData = resourceBuffer->renderResource;

	Shader* dirLight_S = renderData->r_Shader.depthShader.get();
	Shader* pointLight_S = renderData->r_Shader.depthShaderCubeMap.get();
	Shader* spotLight_S = renderData->r_Shader.depthShader_SpotLight.get();

	//DirLight
	if (renderData->lights.dirLightCastShadow) {
		//Resize dir light shadow text if necessary
		Render::Shadow_Quality shadowQuality = renderData->lights.shadow_DirLight.quality;
		Resize_ShadowText(shadowData->dirLight_depthMap, shadowData->dirLight_Current_Quality, shadowQuality);

		dirLight_S->Use();
		glBindFramebuffer(GL_FRAMEBUFFER, shadowData->dirLight_depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);


		int quality_value = Render::ShadowQuality_To_Value(shadowQuality);
		int quality_Index = static_cast<int>(shadowQuality);

		world->renderer->SetViewport_Size(glm::vec2(quality_value, quality_value));
		world->renderer->ExecuteRenderCommands_Shadow(); //RenderCommand per face
	}

	//SpotLight
	spotLight_S->Use();
	for (Padding_SpotLight& currentSpotLight : renderData->lights.spotLights) {
		//Check if the light casts a shadow 
		int shadowIndex = currentSpotLight.shadowIndex;
		if (shadowIndex == -1) continue;

		Render::ShadowMap currentShadow = renderData->lights.shadow_SpotLights[shadowIndex];
		int quality_value = Render::ShadowQuality_To_Value(currentShadow.quality);
		int quality_Index = static_cast<int>(currentShadow.quality);

		//ShadowIndex becomes the position of the first shadow texture for this light
		currentSpotLight.shadowIndex = last_Empty_Slot_Spot[quality_Index];

		//Change FBO and viewport depending on the quality
		glBindFramebuffer(GL_FRAMEBUFFER, shadowData->spot_FBO[quality_Index]);
		world->renderer->SetViewport_Size(glm::vec2(quality_value, quality_value));

		spotLight_S->setInt("currentLight", shadowIndex);

		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowData->spot_Array[quality_Index], 0, last_Empty_Slot_Spot[quality_Index]);
		world->renderer->ExecuteRenderCommands_Shadow();
		last_Empty_Slot_Spot[quality_Index]++;
	}
	//PointLight
	pointLight_S->Use();
	for (Padding_PointLight& currentPointLight : renderData->lights.pointLights) {
		//Check if the light casts a shadow 
		int shadowIndex = currentPointLight.shadowIndex;
		if (shadowIndex == -1) continue;

		Render::ShadowMap_PointLight currentShadow = renderData->lights.shadow_PointLights[shadowIndex];
		int quality_value = Render::ShadowQuality_To_Value(currentShadow.quality);
		int quality_Index = static_cast<int>(currentShadow.quality);

		//ShadowIndex becomes the position of the first shadow texture for this light
		currentPointLight.shadowIndex = last_Empty_Slot_Point[quality_Index];

		//Change FBO and viewport depending on the quality
		glBindFramebuffer(GL_FRAMEBUFFER, shadowData->point_FBO[quality_Index]);
		world->renderer->SetViewport_Size(glm::vec2(quality_value, quality_value));

		pointLight_S->setInt("currentLight", shadowIndex);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowData->point_Array[quality_Index], 0);
		world->renderer->ExecuteRenderCommands_PointShadow(); //RenderCommand per face, 1 Draw per point light
		last_Empty_Slot_Point[quality_Index]++;
	}
	world->renderer->SetViewport_Size(glm::vec2(renderData->renderWIDTH, renderData->renderHEIGHT));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Systems::LightSystem::ShadowPass(World* world, const Resource::ResourceBuffer* resourceBuffer) {
	glCullFace(GL_BACK);
	world->renderer->BuildInstance_ShadowSSBO();

	ClearShadowsMap(world, resourceBuffer);
	UpdateShadow(world, resourceBuffer);
	DrawShadow(world, resourceBuffer);
	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
	world->renderer->ClearShadowOrderList();
}

#pragma endregion

#pragma region Light

#pragma region Init
void Systems::LightSystem::InitLightSSBO(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::RenderResource* renderResource = resourceBuffer->renderResource;

	glGenBuffers(1, &renderResource->light_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderResource->light_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Padding_DirLight) + MAX_POINT_LIGHT * sizeof(Padding_PointLight) + MAX_SPOT_LIGHT * sizeof(Padding_SpotLight) + sizeof(int) * 2, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	// BINDING SSBO
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, LIGHT_BINDING_POINT, renderResource->light_SSBO, 0, sizeof(Padding_DirLight) + MAX_POINT_LIGHT * sizeof(Padding_PointLight) + MAX_SPOT_LIGHT * sizeof(Padding_SpotLight) + sizeof(int) * 2);

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
	int resolution = renderData->skyboxResolution;

	glGenFramebuffers(1, &renderData->captureFBO);
	glGenRenderbuffers(1, &renderData->captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, renderData->captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, renderData->captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderData->captureRBO);

	glGenTextures(1, &renderData->envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
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
	int resolution = renderData->skyboxResolution;

	glGenTextures(1, &renderData->brdfLUTTexture);

	glBindTexture(GL_TEXTURE_2D, renderData->brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, resolution, resolution, 0, GL_RG, GL_FLOAT, 0);
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
	prefilter_Shader->setFloat("resolution", (float)renderData->skyboxResolution);

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
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, renderData->skyboxResolution, renderData->skyboxResolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderData->captureRBO);
	world.renderer->SetViewport_Size(glm::vec2(renderData->skyboxResolution, renderData->skyboxResolution));

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
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, renderData->skyboxResolution, renderData->skyboxResolution);
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

void Systems::LightSystem::UpdateLight(World* world, Resource::RenderResource* renderData) {
	for (auto& spotLight : renderData->lights.spotLights) {  // This value need to be calculate at the last minute
		spotLight.cutOff = glm::cos(glm::radians(spotLight.cutOff));
		spotLight.outerCutOff = glm::cos(glm::radians(spotLight.outerCutOff));
	}

	size_t offset_End_DirLight = renderData->lightSSBO_Data_Size[0];
	size_t offset_End_PointLight = offset_End_DirLight + renderData->lightSSBO_Data_Size[1];
	size_t offset_End_SpotLight = offset_End_PointLight + renderData->lightSSBO_Data_Size[2];

	int activePointLights = std::min((int)renderData->lights.pointLights.size(), MAX_POINT_LIGHT); // Set a maximum of 8
	int activeSpotLights = std::min((int)renderData->lights.spotLights.size(), MAX_SPOT_LIGHT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderData->light_SSBO);


	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, offset_End_DirLight, &renderData->lights.dirLight); // Component::DirLight
	if (!renderData->lights.pointLights.empty()) {
		size_t actual_PointLights_Size = renderData->lights.pointLights.size() * sizeof(Padding_PointLight);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_DirLight, actual_PointLights_Size, renderData->lights.pointLights.data()); // Component::PointLight
	}
	if (!renderData->lights.spotLights.empty()) {
		size_t actual_SpotLights_Size = renderData->lights.spotLights.size() * sizeof(Padding_SpotLight);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_PointLight, actual_SpotLights_Size, renderData->lights.spotLights.data()); // Component::SpotLight
	}

	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_SpotLight, sizeof(int), &activePointLights);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_SpotLight + sizeof(int), sizeof(int), &activeSpotLights);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Systems::LightSystem::UpdateShadow(World* world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::ShadowResource* shadowData = resourceBuffer->shadowResource;
	Resource::RenderResource* renderData = resourceBuffer->renderResource;

	size_t offset_End_DirLight = shadowData->SSBO_Data_Size[0];
	size_t offset_End_PointLight = offset_End_DirLight + shadowData->SSBO_Data_Size[1];
	size_t offset_End_SpotLight = offset_End_PointLight + shadowData->SSBO_Data_Size[2];

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadowData->SSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, offset_End_DirLight, &renderData->lights.shadow_DirLight); //DirLight
	if (!renderData->lights.shadow_PointLights.empty()) {
		size_t actual_Shadow_PointLights_Size = renderData->lights.shadow_PointLights.size() * sizeof(Render::ShadowMap_PointLight);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_DirLight, actual_Shadow_PointLights_Size, renderData->lights.shadow_PointLights.data()); //PointLight
	}
	if (!renderData->lights.shadow_SpotLights.empty()) {
		size_t actual_Shadow_SpotLights_Size = renderData->lights.shadow_SpotLights.size() * sizeof(Render::ShadowMap);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_PointLight, actual_Shadow_SpotLights_Size, renderData->lights.shadow_SpotLights.data()); //SpotLight
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Systems::LightSystem::LightCollector(World* world, Component::CameraComponent* mainCamera, Resource::RenderResource* renderData) {
	renderData->lights.Clear();
	
	View viewDirLight = world->view<Component::DirLight>();

	// Directional light
	int starCompteur = 0;
	viewDirLight.each([&](int entity, Component::DirLight& dirLight) {
		Padding_DirLight p_DirLight;

		starCompteur++;
		if (starCompteur <= 1) {

			//Light
			p_DirLight.color = dirLight.color * dirLight.intensity;
			p_DirLight.direction = dirLight.direction;
			renderData->lights.dirLight = p_DirLight;

			//Shadow
			if (dirLight.shadowCaster.castShadow) {
				//Create a shadowMap struct for the Dir light, use in the Shadow Pass
				Render::ShadowMap shadow_Light(dirLight.shadowCaster.quality);
				shadow_Light.lightSpaceMatrice = dirLight.UpdateMatrix(mainCamera->viewMatrice, renderData->projection);

				renderData->lights.shadow_DirLight = std::move(shadow_Light);
				renderData->lights.dirLightCastShadow = true;
			}
		}
		else {
			std::cout << "Multiple dir light detected, only the first one will be take into consideration" << std::endl;
		}

	});
	// Point lights
	int pointLight_compteur = 0;
	View viewPointLight = world->view<Component::PointLight, Component::Transform>();
	viewPointLight.each([&](int entity, Component::PointLight& pointLight, Component::Transform& transform) {
		pointLight_compteur++;
		if (pointLight_compteur <= MAX_POINT_LIGHT) {
			//Light, Collect light data
			Padding_PointLight p_pointLight;
			p_pointLight.position = transform.position;
			p_pointLight.color = pointLight.color * pointLight.intensity;
			p_pointLight.range = pointLight.range;


			//Shadow, Collect shadow Data
			if (pointLight.shadowCaster.castShadow) {
				//Takes the shadow caster's quality
				Render::Shadow_Quality current_Quality = pointLight.shadowCaster.quality;

				//Create a shadowMap struct for this Point light, use in the Shadow Pass
				Render::ShadowMap_PointLight shadowMap_PointLight(current_Quality, transform.position, pointLight.range);

				//Update light
				//shadowIndex points to the corresponding shadow texture in All_light, its function will change during the current frame
				//shadowQuality is there to tell the shader which list the texture is in
				p_pointLight.shadowQuality = static_cast<int>(current_Quality);
				p_pointLight.shadowIndex = renderData->lights.shadow_PointLights.size();

				//Push the shadow struct
				renderData->lights.shadow_PointLights.push_back(std::move(shadowMap_PointLight));
			}

			renderData->lights.pointLights.push_back(p_pointLight);
		}
		else {
			std::cout << "Component::PointLight number exceed the limit of: " << MAX_POINT_LIGHT << " Some light will not be taking into consideration, current nbr pointLight: " << pointLight_compteur << std::endl;
		}

	});
	// Spot lights
	int spotLight_compteur = 0;
	View viewSpotLight = world->view<Component::SpotLight, Component::Transform>();
	viewSpotLight.each([&](int entity, Component::SpotLight& spotLight, Component::Transform& transform) {
		spotLight_compteur++;
		if (spotLight_compteur <= MAX_SPOT_LIGHT) {
			Padding_SpotLight p_spotLight;
			//Light
			p_spotLight.position = transform.position;
			p_spotLight.direction = Calc_SpotLightDirection(transform.GetTransformModel(), spotLight.direction);
			p_spotLight.color = spotLight.color * spotLight.intensity;
			p_spotLight.range = spotLight.range;
			p_spotLight.cutOff = spotLight.cutOff;
			p_spotLight.outerCutOff = spotLight.outerCutOff;


			//Shadow
			if (spotLight.shadowCaster.castShadow) {
				//Takes the shadow caster's quality
				Render::Shadow_Quality current_Quality = spotLight.shadowCaster.quality;

				//Create a shadowMap struct for this Spot light, use in the Shadow Pass
				Render::ShadowMap shadowMap(spotLight.shadowCaster.quality);
				shadowMap.Generate_SpotLight_ShadowProj(transform.position, spotLight.direction, spotLight.range, spotLight.outerCutOff);

				//Update light
				//shadowIndex points to the corresponding shadow texture in All_light, its function will change during the current frame
				//shadowQuality is there to tell the shader which list the texture is in
				p_spotLight.shadowQuality = static_cast<int>(current_Quality);
				p_spotLight.shadowIndex = renderData->lights.shadow_SpotLights.size();
				spotLight.lightSpaceMatrix = shadowMap.lightSpaceMatrice;

				//Push the shadow struct
				renderData->lights.shadow_SpotLights.push_back(std::move(shadowMap));
			}
			renderData->lights.spotLights.push_back(p_spotLight);
		}
		else {
			std::cout << "Component::SpotLight number exceed the limit of: " << MAX_SPOT_LIGHT << " Some light will not be taking into consideration, current nbr spotLight: " << spotLight_compteur << std::endl;
		}

	});
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

static void DrawPhysicsBox(World* world, Resource::PhysicsResource* physicsData, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
	physicsData->debugJoltShader->Use();
	physicsData->debugJoltShader->setMatrix("view", viewMatrix);
	physicsData->debugJoltShader->setMatrix("projection", projectionMatrix);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	physicsData->joltDebugRenderer->DisplayDebugShape(world);

	//physicsData->physics_system.DrawBodies(physicsData->debug_draw_settings, physicsData->joltDebugRenderer.get());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


//Post process
void Systems::LightSystem::Draw_FinalPass(World* world, Resource::RenderResource* renderData, Resource::PhysicsResource* physicsData, glm::mat4 viewMatrice) {
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
	glBindTexture(GL_TEXTURE_2D, renderData->pingpongBuffers[!renderData->horizontal]); // Last buffer use
	postProcessShader->setInt("bloomBlur", 1);
	postProcessShader->setBool("bloomEnable", renderData->bloomEnable);

	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtColorOutput);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);

	if (physicsData->display_physicsShape) DrawPhysicsBox(world, physicsData, viewMatrice, renderData->projection);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
}

//Lightning Pass
void Systems::LightSystem::LightningPass(World* world, Component::Transform* transformMainCamera, const Resource::ResourceBuffer* resourceBuffer, glm::mat4 viewMatrice) {
	Resource::InterfaceRessource* interfaceRessource = resourceBuffer->interfaceRessource;
	Resource::RenderResource* renderResource = resourceBuffer->renderResource;
	Resource::ShadowResource* shadowResource = resourceBuffer->shadowResource;
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

	//DirLightFBO
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, shadowResource->dirLight_depthMap);
	lighningShader->setInt("dirLight_shadowMap", i++);
	lighningShader->setInt("dirLight_HaveShadow", renderResource->lights.dirLightCastShadow);

	world->renderer->DrawQuad(renderResource);
	//Skybox
	Draw_SkyBox(world, resourceBuffer, viewMatrice);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#pragma endregion


void Systems::LightSystem::Init(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	//static_assert(sizeof(Padding_DirLight) == 32, "Invalide alignement");
	static_assert(alignof(Padding_DirLight) == 16);

	//static_assert(sizeof(Padding_PointLight) == 32, "Invalide alignement");
	static_assert(alignof(Padding_PointLight) == 16);

	//static_assert(sizeof(Padding_SpotLight) == 64, "Invalide alignement");
	static_assert(alignof(Padding_SpotLight) == 16);

	//static_assert(sizeof(ShadowInstanceData) == 80, "Invalide alignement");

	InitLightSSBO(world, resourceBuffer);
	InitShadowSSBO(world, resourceBuffer);
	InitShadowArray(world, resourceBuffer);
	InitDirLightFBO(world, resourceBuffer);
	InitShadowText_SSBO(world, resourceBuffer);
	InitShadowFBO(world, resourceBuffer);

	InitCaptureCubeMap(world, resourceBuffer);
	Init_IrradianceMap(world, resourceBuffer);
	Init_BRDF_LUTTexture(world, resourceBuffer);

	Equirenctangular_To_CubeMap(world, resourceBuffer, "SkyBox/inTheSky/kloofendal_48d_partly_cloudy_puresky_2k.hdr");  //qwantani_night_puresky_2k
}

void Systems::LightSystem::Update(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::RenderResource* renderResource = world.get_ressource<Resource::RenderResource>();
	Resource::WindowResource* windowResource = world.get_ressource<Resource::WindowResource>();
	Resource::PhysicsResource* physicsResource = world.get_ressource<Resource::PhysicsResource>();
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

	LightCollector(&world, mainCamera, renderResource);

	ShadowPass(&world, resourceBuffer);
	UpdateLight(&world, renderResource);
	glDisable(GL_CULL_FACE);
	LightningPass(&world, transformMainCamera, resourceBuffer, mainCamera->viewMatrice);
	Draw_FinalPass(&world, renderResource, physicsResource, mainCamera->viewMatrice);
	glEnable(GL_CULL_FACE);

	//glfwSwapBuffers(windowResource->window);
	glfwPollEvents();
}