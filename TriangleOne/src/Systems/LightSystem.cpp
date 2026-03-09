  #include "Systems/LightSystem.h"

void LightSystem::Init(World& world) {
	static_assert(sizeof(Padding_DirLight) == 64, "Invalide alignement");
	static_assert(alignof(Padding_DirLight) == 16);

	static_assert(sizeof(Padding_PointLight) == 80, "Invalide alignement");
	static_assert(alignof(Padding_PointLight) == 16);

	static_assert(sizeof(Padding_SpotLight) == 96, "Invalide alignement");
	static_assert(alignof(Padding_SpotLight) == 16);
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
	SendDepthMapToMainShader(&world, resourceBuffer, lights);
	

	delete lights;  // WARNING, in case for some misc reason lights is delete before all values are copy in the gpu
}

#pragma region Init shadow buffer 
void LightSystem::InitShadowMap(DirLight* currentLight) {
	glGenFramebuffers(1, &currentLight->depthMapFBO);
	
	glGenTextures(1, &currentLight->depthMap);
	glBindTexture(GL_TEXTURE_2D, currentLight->depthMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, currentLight->SHADOW_WIDTH, currentLight->SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

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

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

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
				break;
			}
		}
		case LightTag::SpotLight_Tag: {
			SpotLight* currentLight = world.get_component<SpotLight>(entity);
			if (currentLight != nullptr) {
				InitSpotShadowMap(currentLight);
				break;
			}
		}
		case LightTag::Directional_Tag: {
			DirLight* currentLight = world.get_component<DirLight>(entity);
			if (currentLight != nullptr) {
				InitShadowMap(currentLight);
				break;
			}
		}
		default:
			assert(true, "Unexpected Error in InitShadowBuffer");
			break;
		}
		world.remove_component<LightToInitTag>(entity);
	});
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

	View view = world->view<ModeleHandle, SceneTag, Transform>();
	view.each([&](int entity, ModeleHandle& modeleHandle, SceneTag& sceneTag, Transform& transform) {
		if (modeleHandle.haveToBeDraw && modeleHandle.castShadow && sceneTag.scene_id == 0) {
			Model currentModel = world->modelStore->Get_Model(modeleHandle.index);

			depthShader->setMatrix("model", transform.GetTransformModel());
			currentModel.DrawWithoutTexture(depthShader);
		}

	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowData.WIDHT, windowData.HEIGHT);
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


	View view = world->view<ModeleHandle, SceneTag, Transform>();
	view.each([&](int entity, ModeleHandle& modeleHandle, SceneTag& sceneTag, Transform& transform) {
		if (modeleHandle.haveToBeDraw && modeleHandle.castShadow && sceneTag.scene_id == 0) {
			Model currentModel = world->modelStore->Get_Model(modeleHandle.index);

			depthShader->setMatrix("model", transform.GetTransformModel());
			currentModel.DrawWithoutTexture(depthShader);
		}
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowData.WIDHT, windowData.HEIGHT);
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
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


	depthShader->Use();
	depthShader->setMatrix("lightSpaceMatrix", lightSpaceMatrix);


	View view = world->view<ModeleHandle, SceneTag, Transform>();
	view.each([&](int entity, ModeleHandle& modeleHandle, SceneTag& sceneTag, Transform& transform) {
		if (modeleHandle.castShadow && sceneTag.scene_id == 0) {
			Model currentModel = world->modelStore->Get_Model(modeleHandle.index);

			depthShader->setMatrix("model", transform.GetTransformModel());
			currentModel.DrawWithoutTexture(depthShader);
		}
	});


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowData.WIDHT, windowData.HEIGHT);
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
void LightSystem::UpdateLight(World* world, RenderResource* renderResource, All_Light& lights) {

	//New Methode UBO, i know its not better that a differed lightning system
	//glBindBuffer(GL_UNIFORM_BUFFER, renderResource->light_UBO);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Padding_DirLight), &lights.dirLight); // DirLight
	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Padding_DirLight), MAX_POINT_LIGHT * sizeof(Padding_PointLight), lights.pointLights.data()); // PointLight
	//glBufferSubData(GL_UNIFORM_BUFFER, MAX_POINT_LIGHT * sizeof(Padding_PointLight), MAX_SPOT_LIGHT * sizeof(Padding_SpotLight), lights.spotLights.data()); // SpotLight
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//

	for (auto& material : world->modelStore->materials) {
		Shader* shader = &material.shader;
		shader->Use();

		//Directional light
		shader->setVec3("dirLight.direction", glm::normalize(lights.dirLight.direction));
		shader->setVec3("dirLight.ambient", lights.dirLight.ambient);
		shader->setVec3("dirLight.diffuse", lights.dirLight.diffuse);
		shader->setVec3("dirLight.specular", lights.dirLight.specular);

		for (int i = 0; i < lights.pointLights.size(); i++) {
			shader->setVec3("pointLights[" + std::to_string(i) + "].position", lights.pointLights[i].position);

			shader->setVec3("pointLights[" + std::to_string(i) + "].ambient", lights.pointLights[i].ambient);
			shader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", lights.pointLights[i].diffuse);
			shader->setVec3("pointLights[" + std::to_string(i) + "].specular", lights.pointLights[i].specular);

			shader->setFloat("pointLights[" + std::to_string(i) + "].range", lights.pointLights[i].range);

		}
		for (int i = 0; i < lights.spotLights.size(); i++) {
			shader->setVec3("spotLights[" + std::to_string(i) + "].position", lights.spotLights[i].position);


			shader->setVec3("spotLights[" + std::to_string(i) + "].direction", lights.spotLights[i].direction);

			shader->setVec3("spotLights[" + std::to_string(i) + "].ambient", lights.spotLights[i].ambient);
			shader->setVec3("spotLights[" + std::to_string(i) + "].diffuse", lights.spotLights[i].diffuse);
			shader->setVec3("spotLights[" + std::to_string(i) + "].specular", lights.spotLights[i].specular);

			shader->setFloat("spotLights[" + std::to_string(i) + "].cutOff", glm::cos(glm::radians(lights.spotLights[i].cutOff)));
			shader->setFloat("spotLights[" + std::to_string(i) + "].outerCutOff", glm::cos(glm::radians(lights.spotLights[i].outerCutOff)));
			shader->setFloat("spotLights[" + std::to_string(i) + "].range", lights.spotLights[i].range);
		}
		int activePointLights = std::min((int)lights.pointLights.size(), 8); // Bloquer à 8 max
		int activeSpotLights = std::min((int)lights.spotLights.size(), 8); // Bloquer à 8 max

		shader->setInt("nbrPointLight", activePointLights);
		shader->setInt("nbrSpotLight", activeSpotLights);
	}

}


glm::vec3 LightSystem::Calc_SpotLightDirection(glm::mat4 transformModel, glm::vec3 lightDirection) {
	return glm::normalize(glm::vec3(transformModel * glm::vec4(lightDirection, 0.0f)));
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
			glm::mat4 projectionCamera = glm::perspective(glm::radians(mainCamera->zoom), (float)windowResource->WIDHT / (float)windowResource->HEIGHT, mainCamera->nearPlane, mainCamera->farPlane);
			lights->dirLight_Matrice = dirLight.UpdateMatrix(mainCamera->viewMatrice, projectionCamera);
		}
		else {
			std::cout << "Multiple dir light detected, only the first one will be take into consideration" << std::endl;
		}

		});

	View viewPointLight = world->view<PointLight, Transform>();
	viewPointLight.each([&](int entity, PointLight& pointLight, Transform& transform) {
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
		});

	View viewSpotLight = world->view<SpotLight, Transform>();
	viewSpotLight.each([&](int entity, SpotLight& spotLight, Transform& transform) {
		Padding_SpotLight p_spotLight;
		//Spot_Light
		p_spotLight.position = transform.position;
		p_spotLight.direction = Calc_SpotLightDirection(transform.GetTransformModel(), spotLight.direction);
		p_spotLight.ambient = spotLight.ambient;
		p_spotLight.diffuse = spotLight.diffuse;
		p_spotLight.specular = spotLight.specular;
		p_spotLight.range = spotLight.range;
		lights->spotLights.push_back(p_spotLight);

		//Shadow
		lights->spotLights_DepthMap.push_back(spotLight.depthMap);
		lights->spotLights_DepthMapFBO.push_back(spotLight.depthMapFBO);
		lights->spotLights_Shadow_Size.push_back(std::make_pair(spotLight.SHADOW_WIDTH, spotLight.SHADOW_HEIGHT));

		//Matrices
		lights->spotLight_Matrice.push_back(spotLight.lightSpaceMatrix);
		});

	return lights;
}


#pragma endregion

void LightSystem::SendDepthMapToMainShader(World* world, const ResourceBuffer* resourceBuffer, All_Light* lights) {  // Temp
	if (lights->pointLights_DepthMap.size() >= 8) std::cout << "Max pointLight number reach" << std::endl;  // Valeur a definir a l'avenir dans un dossier config
	if (lights->spotLights_DepthMap.size() >= 8) std::cout << "Max spotLight number reach" << std::endl;  // Valeur a definir a l'avenir dans un dossier config

 
	View view = world->view<ModeleHandle, Transform, MaterialHandle>();  // Temporary view, gonna be UBO in the futur 
	view.each([&](int entity, ModeleHandle& modeleHandle, Transform& transform, MaterialHandle& materialHandle) {
		if (modeleHandle.castShadow) {
			Shader currentShader = world->modelStore->Get_Material(materialHandle.index).shader;
			currentShader.Use();
			// --- TEXTURE UNIT MANAGEMENT ---

			const int SLOT_SHADOW_DIR = 16;
			const int SLOT_SHADOW_POINT_START = 17;
			const int SLOT_SHADOW_SPOT_START = 25;

			// --- 2. GESTION LUMIERE DIRECTIONNELLE (Shadow Map 2D) ---
			currentShader.setInt("shadowMap", SLOT_SHADOW_DIR);

			glActiveTexture(GL_TEXTURE0 + SLOT_SHADOW_DIR);
			if (lights->dirLight_DepthMap != 0) {  // WARNING, do a better check if the sun existe
				glBindTexture(GL_TEXTURE_2D, lights->dirLight_DepthMap); 
				currentShader.setMatrix("lightSpaceMatrix", lights->dirLight_Matrice);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			// --- 3. GESTION POINT LIGHTS (Shadow Cube Maps) ---
			int maxPointLights = 8;
			int activeLights = std::min((int)lights->pointLights_DepthMap.size(), maxPointLights);
			currentShader.setInt("nbrPointLight", activeLights);

			
			//PointLight
			for (int i = 0; i < maxPointLights; i++) {
				if (i > lights->pointLights_DepthMap.size() - 1) break;

				// Construction du nom "shadowCubeMaps[0]", "shadowCubeMaps[1]"...
				std::string uniformName = "shadowCubeMaps[" + std::to_string(i) + "]";

				int currentSlot = SLOT_SHADOW_POINT_START + i;

				// 1. On dit au shader : "Le sampler i doit lire dans le slot X"
				currentShader.setInt(uniformName, currentSlot);

				// 2. On active le slot X
				glActiveTexture(GL_TEXTURE0 + currentSlot);

				if (i < activeLights) {
					glBindTexture(GL_TEXTURE_CUBE_MAP, lights->pointLights_DepthMap[i]);
				}
				else {
					// Nettoyage des slots inutilisés (évite les bugs de "Sampler Type Mismatch")
					glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				}
			}

			//// Gestion Spot Light
			int maxSpotLights = 8;
			int activeSpotLights = std::min((int)lights->spotLights_DepthMap.size(), maxSpotLights);
			for (int i = 0; i < maxSpotLights; i++) {
				std::string uniformName = "shadowMapSpot[" + std::to_string(i) + "]";
				std::string uniformNameMatrice = "spotLightMatrices[" + std::to_string(i) + "]";

				int currentSlot = SLOT_SHADOW_SPOT_START + i;

				currentShader.setInt(uniformName, currentSlot);


				glActiveTexture(GL_TEXTURE0 + currentSlot);


				if (i < activeSpotLights) {
					currentShader.setMatrix(uniformNameMatrice, lights->spotLight_Matrice[i]);
					glBindTexture(GL_TEXTURE_2D, lights->spotLights_DepthMap[i]);
				}
				else {
					glBindTexture(GL_TEXTURE_2D, 0);
				}
			}

			glActiveTexture(GL_TEXTURE0);

			// --- END TEXTURE MANAGEMENT ---
		}
	});
}

void LightSystem::InitLightUbo(const ResourceBuffer* resourceBuffer) {
	RenderResource* renderResource = resourceBuffer->renderResource;

	glGenBuffers(1, &renderResource->light_UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, renderResource->light_UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Padding_DirLight) + MAX_POINT_LIGHT * sizeof(Padding_PointLight) + MAX_SPOT_LIGHT * sizeof(Padding_SpotLight), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// BINDING UBO slot 1, light
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, renderResource->light_UBO, 0, sizeof(Padding_DirLight) + MAX_POINT_LIGHT * sizeof(Padding_PointLight) + MAX_SPOT_LIGHT * sizeof(Padding_SpotLight));


}
