  #include "Systems/LightSystem.h"

#pragma region Init shadow buffer 

void LightSystem::InitShadowMap(DirLight* currentLight) {
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

void LightSystem::InitCubeMap(PointLight* currentLight) {

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

void LightSystem::InitSpotShadowMap(SpotLight* currentLight) {

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

void LightSystem::InitShadowBuffer(World& world) {
	std::vector<int> to_remove;
	View view = world.view<LightToInitTag>();
	view.each([&](int entity, LightToInitTag& lightTag) {
		switch (lightTag.tag)
		{
		case LightTag::None:
			assert(true, "Init Tag not define on a light");
			break;
		case LightTag::PointLight_Tag: {
			PointLight* currentLight = world.get_component<PointLight>(entity);
			if (currentLight != nullptr) {
				InitCubeMap(currentLight);
				to_remove.push_back(entity);
				break;
			}
		}
		case LightTag::SpotLight_Tag: {
			SpotLight* currentLight = world.get_component<SpotLight>(entity);
			if (currentLight != nullptr) {
				InitSpotShadowMap(currentLight);
				to_remove.push_back(entity);
				break;
			}
		}
		case LightTag::Directional_Tag: {
			DirLight* currentLight = world.get_component<DirLight>(entity);
			if (currentLight != nullptr) {
				InitShadowMap(currentLight);
				to_remove.push_back(entity);
				break;
			}
		}
		default:
			assert(true, "Unexpected Error in InitShadowBuffer");
			break;
		}
	});
	for (int entity : to_remove) {
		world.remove_component<LightToInitTag>(entity);
	}
}

#pragma endregion 

#pragma region Draw Shadow

void LightSystem::DrawShadowForDirLight(World* world, RenderResource& renderResource, WindowResource& windowData, All_Light& lights) {  // Bug sur la window si resize
	glViewport(0, 0, lights.dirLight_Shadow_Size.first, lights.dirLight_Shadow_Size.second);
	glBindFramebuffer(GL_FRAMEBUFFER, lights.dirLight_DepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	Shader* depthShader = renderResource.depthShader.get();

	depthShader->Use();
	depthShader->setMatrix("lightSpaceMatrix", lights.dirLight_Matrice);

	View view = world->view<MeshHandle, SceneTag, Transform>();
	view.each([&](int entity, MeshHandle& meshHandle, SceneTag& sceneTag, Transform& transform) {
		if (meshHandle.haveToBeDraw && meshHandle.castShadow && sceneTag.scene_id == 0) {
			Mesh currentMesh = world->assetStore->Get_Mesh(meshHandle.index);

			depthShader->setMatrix("model", transform.GetTransformModel());
			world->renderer->DrawMesh_Without_Texture(currentMesh);
		}

	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowData.WIDTH, windowData.HEIGHT);
}

void LightSystem::DrawShadowForPointLight(World* world, RenderResource& renderResource, WindowResource& windowData, All_Light& lights, int index) {
	std::pair<unsigned int, unsigned int> shadowSize = lights.pointLights_Shadow_Size[index];
	glm::vec3 position = lights.pointLights[index].position;
	Shader* depthShader = renderResource.depthShaderCubeMap.get();
	float range = lights.pointLights[index].range;

	float aspect = (float)shadowSize.first / (float)shadowSize.second;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, 0.1f, range);


	glViewport(0, 0, shadowSize.first, shadowSize.second);
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
	depthShader->setVec3("lightPos", position);


	for (int i = 0; i < shadowTransforms.size(); i++) {
		depthShader->setMatrix("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	}


	View view = world->view<MeshHandle, SceneTag, Transform>();
	view.each([&](int entity, MeshHandle& meshHandle, SceneTag& sceneTag, Transform& transform) {
		if (meshHandle.haveToBeDraw && meshHandle.castShadow && sceneTag.scene_id == 0) {
			Mesh currentMesh = world->assetStore->Get_Mesh(meshHandle.index);

			depthShader->setMatrix("model", transform.GetTransformModel());
			world->renderer->DrawMesh_Without_Texture(currentMesh);
		}
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowData.WIDTH, windowData.HEIGHT);
}

void LightSystem::DrawShadowForSpotLight(World* world, RenderResource& renderResource, WindowResource& windowData, All_Light& lights, int index) {
	std::pair<unsigned int, unsigned int> shadowSize = lights.spotLights_Shadow_Size[index];
	glm::vec3 position = lights.spotLights[index].position;
	glm::vec3 direction = lights.spotLights[index].direction;
	Shader* depthShader = renderResource.depthShader.get();


	float aspect = (float)shadowSize.first / (float)shadowSize.second;
	glm::mat4 shadowProj = glm::perspective(glm::radians(lights.spotLights[index].outerCutOff * 2.0f), aspect, 0.1f, lights.spotLights[index].range);

	glm::vec3 up = (glm::abs(direction.y) > 0.99f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
	glm::mat4 shadowView = glm::lookAt(position, position + direction, up);


	glm::mat4 lightSpaceMatrix = shadowProj * shadowView;

	glViewport(0, 0, shadowSize.first, shadowSize.second);
	glBindFramebuffer(GL_FRAMEBUFFER, lights.spotLights_DepthMapFBO[index]);  // Fbo unique par spot light
	glClear(GL_DEPTH_BUFFER_BIT );


	depthShader->Use();
	depthShader->setMatrix("lightSpaceMatrix", lightSpaceMatrix);


	View view = world->view<MeshHandle, SceneTag, Transform>();
	view.each([&](int entity, MeshHandle& meshHandle, SceneTag& sceneTag, Transform& transform) {
		if (meshHandle.castShadow && sceneTag.scene_id == 0) {
			Mesh currentMesh = world->assetStore->Get_Mesh(meshHandle.index);

			depthShader->setMatrix("model", transform.GetTransformModel());
			world->renderer->DrawMesh_Without_Texture(currentMesh);
		}
	});


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowData.WIDTH, windowData.HEIGHT);
}


void LightSystem::ShadowPass(World* world, RenderResource* renderResource, WindowResource* windowResource, All_Light* lights) {
	glCullFace(GL_FRONT);
	DrawShadowForDirLight(world, *renderResource, *windowResource, *lights);

	for (int i = 0; i < lights->pointLights.size(); i++) {
		DrawShadowForPointLight(world, *renderResource, *windowResource, *lights, i);
	}

	for (int i = 0; i < lights->spotLights.size(); i++) {
		DrawShadowForSpotLight(world, *renderResource, *windowResource, *lights, i);
	}
	glCullFace(GL_BACK);
}

#pragma endregion

#pragma region Light

void LightSystem::InitLightSSBO(World& world, const ResourceBuffer* resourceBuffer) {
	int uniform_Light_Binding_Point = 1;
	RenderResource* renderResource = resourceBuffer->renderResource;

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

glm::vec3 LightSystem::Calc_SpotLightDirection(glm::mat4 transformModel, glm::vec3 lightDirection) {
	return glm::normalize(glm::vec3(transformModel * glm::vec4(lightDirection, 0.0f)));
}

void LightSystem::UpdateLight(World* world, RenderResource* renderResource, All_Light& lights) {
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


	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, offset_End_DirLight, &lights.dirLight); // DirLight
	if (!lights.pointLights.empty()) {
		size_t actual_PointLights_Size = lights.pointLights.size() * sizeof(Padding_PointLight);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_DirLight, actual_PointLights_Size, lights.pointLights.data()); // PointLight
	}
	if (!lights.spotLights.empty()) {
		size_t actual_SpotLights_Size = lights.spotLights.size() * sizeof(Padding_SpotLight);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_PointLight, actual_SpotLights_Size, lights.spotLights.data()); // SpotLight
	}

	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_SpotLight, sizeof(int), &activePointLights);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset_End_SpotLight + sizeof(int), sizeof(int), &activeSpotLights);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

All_Light* LightSystem::DataCollector(World* world, WindowResource* windowResource, CameraComponent* mainCamera) {
	All_Light* lights = new All_Light();
	View viewDirLight = world->view<DirLight>();

	int starCompteur = 0;

	viewDirLight.each([&](int entity, DirLight& dirLight) {
		Padding_DirLight p_DirLight;

		starCompteur++;
		if (starCompteur <= 1) {

			//Dir_Light
			p_DirLight.ambient = dirLight.ambient;
			p_DirLight.diffuse = dirLight.diffuse;
			p_DirLight.direction = dirLight.direction;
			p_DirLight.specular = dirLight.specular;
			lights->dirLight = p_DirLight;

			//Shadow
			lights->dirLight_DepthMap = dirLight.depthMap;
			lights->dirLight_DepthMapFBO = dirLight.depthMapFBO;
			lights->dirLight_Shadow_Size = std::make_pair(dirLight.SHADOW_WIDTH, dirLight.SHADOW_HEIGHT);

			//Matrices
			glm::mat4 projectionCamera = glm::perspective(glm::radians(mainCamera->zoom), (float)windowResource->WIDTH / (float)windowResource->HEIGHT, mainCamera->nearPlane, mainCamera->farPlane);
			lights->dirLight_Matrice = dirLight.UpdateMatrix(mainCamera->viewMatrice, projectionCamera);
		}
		else {
			std::cout << "Multiple dir light detected, only the first one will be take into consideration" << std::endl;
		}

		});
	int pointLight_compteur = 0;
	View viewPointLight = world->view<PointLight, Transform>();
	viewPointLight.each([&](int entity, PointLight& pointLight, Transform& transform) {
		pointLight_compteur++;
		if (pointLight_compteur <= MAX_POINT_LIGHT) {
			Padding_PointLight p_pointLight;
			//Point_Light
			p_pointLight.position = transform.position;
			p_pointLight.ambient = pointLight.ambient;
			p_pointLight.diffuse = pointLight.diffuse;
			p_pointLight.specular = pointLight.specular;
			p_pointLight.range = pointLight.range;
			lights->pointLights.push_back(p_pointLight);

			//Shadow
			lights->pointLights_DepthMap.push_back(pointLight.depthCubeMap);
			lights->pointLights_DepthMapFBO.push_back(pointLight.depthCubeMapFBO);
			lights->pointLights_Shadow_Size.push_back(std::make_pair(pointLight.SHADOW_WIDTH, pointLight.SHADOW_HEIGHT));
		}
		else {
			std::cout << "PointLight number exceed the limit of: " << MAX_POINT_LIGHT << " Some light will not be taking into consideration, current nbr pointLight: " << pointLight_compteur << std::endl;
		}

		});
	int spotLight_compteur = 0;
	View viewSpotLight = world->view<SpotLight, Transform>();
	viewSpotLight.each([&](int entity, SpotLight& spotLight, Transform& transform) {
		if (spotLight_compteur <= MAX_POINT_LIGHT) {
			Padding_SpotLight p_spotLight;
			//Spot_Light
			p_spotLight.position = transform.position;
			p_spotLight.direction = Calc_SpotLightDirection(transform.GetTransformModel(), spotLight.direction);
			p_spotLight.ambient = spotLight.ambient;
			p_spotLight.diffuse = spotLight.diffuse;
			p_spotLight.specular = spotLight.specular;
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
			std::cout << "SpotLight number exceed the limit of: " << MAX_SPOT_LIGHT << " Some light will not be taking into consideration, current nbr spotLight: " << spotLight_compteur << std::endl;
		}

		});

	return lights;
}

void LightSystem::LightningPass(World* world, RenderResource* renderResource) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderResource->lightningPass_Shader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderResource->gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, renderResource->gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, renderResource->gAlbedo);
}

#pragma endregion

void LightSystem::SendDepthMapToLightningShader(World* world, const RenderResource* renderResource, const ResourceBuffer* resourceBuffer, All_Light* lights) {
	if (lights->pointLights_DepthMap.size() >= MAX_POINT_LIGHT) std::cout << "Max pointLight number reach" << std::endl; 
	if (lights->spotLights_DepthMap.size() >= MAX_SPOT_LIGHT) std::cout << "Max spotLight number reach" << std::endl;  


	Shader* currentShader = renderResource->lightningPass_Shader.get();
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


	//PointLight
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


void LightSystem::Init(World& world, const ResourceBuffer* resourceBuffer) {
	static_assert(sizeof(Padding_DirLight) == 64, "Invalide alignement");
	static_assert(alignof(Padding_DirLight) == 16);

	//static_assert(sizeof(Padding_PointLight) == 80, "Invalide alignement");
	//static_assert(alignof(Padding_PointLight) == 16);

	static_assert(sizeof(Padding_SpotLight) == 96, "Invalide alignement");
	static_assert(alignof(Padding_SpotLight) == 16);

	InitLightSSBO(world, resourceBuffer);
}

void LightSystem::Update(World& world, const ResourceBuffer* resourceBuffer) {
	InitShadowBuffer(world);

	RenderResource* renderResource = world.get_ressource<RenderResource>();
	WindowResource* windowResource = world.get_ressource<WindowResource>();
	Entity entityCam = resourceBuffer->activeCamera->cameraID;
	CameraComponent* mainCamera = world.get_component<CameraComponent>(entityCam);


	All_Light* lights = DataCollector(&world, windowResource, mainCamera);

	ShadowPass(&world, renderResource, windowResource, lights);
	UpdateLight(&world, renderResource, *lights);
	SendDepthMapToLightningShader(&world, renderResource, resourceBuffer, lights);
	LightningPass(&world, renderResource);

	delete lights;  // WARNING, in case for some misc reason lights is delete before all values are copy in the gpu
}