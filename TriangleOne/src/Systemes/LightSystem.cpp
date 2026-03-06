  #include "Systemes/LightSystem.h"

void LightSystem::Update(World& world, const ResourceBuffer* resourceBuffer) {
	InitShadowBuffer(world);

	UpdateShadow(&world, resourceBuffer);



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
	view.each([&world, this](int entity, LightToInitTag& lightTag) {
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

void LightSystem::DrawShadowForDirLight(World* world, WindowResource* windowData, DirLight* currentLight) {  // Bug sur la window si resize
	glViewport(0, 0, currentLight->SHADOW_WIDTH, currentLight->SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, currentLight->depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	currentLight->depthShader->Use();
	currentLight->depthShader->setMatrix("lightSpaceMatrix", currentLight->lightMatrice);

	View view = world->view<ModeleHandle, SceneTag, Transform>();
	view.each([this, world, currentLight](int entity, ModeleHandle* ModeleHandle, SceneTag& sceneTag, Transform& transform) {
		if (ModeleHandle->haveToBeDraw && ModeleHandle->castShadow && sceneTag.scene_id == 0) {
			Model currentModel = world->modelStore->Get_Model(ModeleHandle->index);

			currentLight->depthShader->setMatrix("model", transform.GetTransformModel());
			currentModel.DrawWithoutTexture(currentLight->depthShader);
		}

	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowData->WIDHT, windowData->HEIGHT);
}

void LightSystem::DrawShadowForPointLight(World* world, WindowResource* windowData, PointLight* currentLight, Transform* transform) {
	currentLight->aspect = (float)currentLight->SHADOW_WIDTH / (float)currentLight->SHADOW_HEIGHT;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), currentLight->aspect, currentLight->near_plane, currentLight->range);


	glViewport(0, 0, currentLight->SHADOW_WIDTH, currentLight->SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, currentLight->depthCubeMapFBO);  // Fbo unique par point light
	glClear(GL_DEPTH_BUFFER_BIT);


	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(transform->position, transform->position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(transform->position, transform->position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(transform->position, transform->position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));


	currentLight->depthShader->Use();
	currentLight->depthShader->setFloat("far_plane", currentLight->range);
	currentLight->depthShader->setVec3("lightPos", transform->position);


	for (int i = 0; i < shadowTransforms.size(); i++) {
		currentLight->depthShader->setMatrix("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	}


	View view = world->view<ModeleHandle, SceneTag, Transform>();
	view.each([this, world, currentLight](int entity, ModeleHandle& ModeleHandle, SceneTag& sceneTag, Transform& transform) {
		if (ModeleHandle.castShadow && sceneTag.scene_id == 0) {
			Model currentModel = world->modelStore->Get_Model(ModeleHandle.index);

			currentLight->depthShader->setMatrix("model", transform.GetTransformModel());
			currentModel.DrawWithoutTexture(currentLight->depthShader);
		}
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowData->WIDHT, windowData->HEIGHT);
}

void LightSystem::DrawShadowForSpotLight(World* world, WindowResource* windowData, SpotLight* currentLight, Transform* transform) {
	currentLight->aspect = (float)currentLight->SHADOW_WIDTH / (float)currentLight->SHADOW_HEIGHT;
	glm::mat4 shadowProj = glm::perspective(glm::radians(currentLight->outerCutOff * 2.0f), currentLight->aspect, 0.1f, currentLight->range);

	glm::vec3 up = (glm::abs(currentLight->direction.y) > 0.99f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
	glm::mat4 shadowView = glm::lookAt(transform->position, transform->position + currentLight->direction, up);


	glm::mat4 lightSpaceMatrix = shadowProj * shadowView;
	currentLight->lightSpaceMatrix = lightSpaceMatrix;

	glViewport(0, 0, currentLight->SHADOW_WIDTH, currentLight->SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, currentLight->depthMapFBO);  // Fbo unique par spot light
	glClear(GL_DEPTH_BUFFER_BIT);


	currentLight->depthShader->Use();
	currentLight->depthShader->setMatrix("lightSpaceMatrix", lightSpaceMatrix);


	View view = world->view<ModeleHandle, SceneTag, Transform>();
	view.each([this, world, currentLight](int entity, ModeleHandle& modeleHandle, SceneTag& sceneTag, Transform& transform) {
		if (modeleHandle.castShadow && sceneTag.scene_id == 0) {
			Model currentModel = world->modelStore->Get_Model(modeleHandle.index);

			currentLight->depthShader->setMatrix("model", transform.GetTransformModel());
			currentModel.DrawWithoutTexture(currentLight->depthShader);
		}
	});


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowData->WIDHT, windowData->HEIGHT);
}


void LightSystem::UpdateShadow(World* world, const ResourceBuffer* resourceBuffer) {
	WindowResource* windowResource = world->get_ressource<WindowResource>();

	std::vector<unsigned int> stars_DepthMap;
	std::vector<unsigned int> pointLights_DepthMap;
	std::vector<unsigned int> spotLights_DepthMap;


	glCullFace(GL_FRONT);
	View viewDirLight = world->view<DirLight>();
	viewDirLight.each([this, world, windowResource, &stars_DepthMap](int entity, DirLight& dirLight) {
		DrawShadowForDirLight(world, windowResource, &dirLight);
		stars_DepthMap.push_back(dirLight.depthMap);
	});

	View viewPointLight = world->view<PointLight, Transform>();
	viewPointLight.each([this, world, windowResource, &pointLights_DepthMap](int entity, PointLight& pointLight, Transform& transform) {
		DrawShadowForPointLight(world, windowResource, &pointLight, &transform);
		pointLights_DepthMap.push_back(pointLight.depthCubeMap);
	});

	View viewSpotLight = world->view<SpotLight, Transform>();
	viewSpotLight.each([this, world, windowResource, &spotLights_DepthMap](int entity, SpotLight& spotLight, Transform& transform) {
		DrawShadowForSpotLight(world, windowResource, &spotLight, &transform);
		spotLights_DepthMap.push_back(spotLight.depthMap);
	});

	glCullFace(GL_BACK);


}

#pragma endregion

void SendDepthMapToMainShader(World* world, const ResourceBuffer* resourceBuffer,
	std::vector<unsigned int> stars_DepthMap,
	std::vector<unsigned int> pointLights_DepthMap,
	std::vector<unsigned int> spotLights_DepthMap) {  // Temp

	if (pointLights_DepthMap.size() >= 8) std::cout << "Max pointLight number reach" << std::endl;  // Valeur a definir a l'avenir dans un dossier config
	if (spotLights_DepthMap.size() >= 8) std::cout << "Max spotLight number reach" << std::endl;  // Valeur a definir a l'avenir dans un dossier config

	View view = world->view<ModeleHandle>();
 

	View view = world->view<ModeleHandle, Transform>();
	view.each([this, world](ModeleHandle& modeleHandle, Transform& transform) {


	});

	for (const auto& currentEntity : scene->GetEntities()) {
		if (!currentEntity->HasComponent<MeshComponent>()) {
			continue;
		}
		MeshComponent* currentModel = currentEntity->GetComponent<MeshComponent>();
		std::shared_ptr<Shader> shader = currentModel->GetShader();

		if (currentModel->haveToBeDraw) {
			shader->Use();

			// --- TEXTURE UNIT MANAGEMENT ---

			const int SLOT_SHADOW_DIR = 16;
			const int SLOT_SHADOW_POINT_START = 17;
			const int SLOT_SHADOW_SPOT_START = 25;

			// --- 2. GESTION LUMIERE DIRECTIONNELLE (Shadow Map 2D) ---
			shader->setInt("shadowMap", SLOT_SHADOW_DIR);

			glActiveTexture(GL_TEXTURE0 + SLOT_SHADOW_DIR);
			if (stars_DepthMap.size() > 0) {
				glBindTexture(GL_TEXTURE_2D, renderData->depthMap);
				shader->setMatrix("lightSpaceMatrix", star.at(0)->lightMatrice);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			// --- 3. GESTION POINT LIGHTS (Shadow Cube Maps) ---
			int maxPointLights = 8;
			int activeLights = std::min((int)pointLights_DepthMap.size(), maxPointLights);
			shader->setInt("nbrPointLight", activeLights);

			//PointLight
			for (int i = 0; i < maxPointLights; i++) {
				// Construction du nom "shadowCubeMaps[0]", "shadowCubeMaps[1]"...
				std::string uniformName = "shadowCubeMaps[" + std::to_string(i) + "]";

				int currentSlot = SLOT_SHADOW_POINT_START + i;

				// 1. On dit au shader : "Le sampler i doit lire dans le slot X"
				shader->setInt(uniformName, currentSlot);

				// 2. On active le slot X
				glActiveTexture(GL_TEXTURE0 + currentSlot);

				if (i < activeLights) {
					glBindTexture(GL_TEXTURE_CUBE_MAP, pointLights_DepthMap[i]);
				}
				else {
					// Nettoyage des slots inutilisés (évite les bugs de "Sampler Type Mismatch")
					glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				}
			}

			//// Gestion Spot Light
			int maxSpotLights = 8;
			for (int i = 0; i < maxSpotLights; i++) {
				std::string uniformName = "shadowMapSpot[" + std::to_string(i) + "]";
				std::string uniformNameMatrice = "spotLightMatrices[" + std::to_string(i) + "]";

				int currentSlot = SLOT_SHADOW_SPOT_START + i;

				shader->setInt(uniformName, currentSlot);


				glActiveTexture(GL_TEXTURE0 + currentSlot);


				if (i < activeLights) {
					shader->setMatrix(uniformNameMatrice, spotLights[i].first->lightSpaceMatrix);
					glBindTexture(GL_TEXTURE_2D, spotLights[i].first->depthMap);
				}
				else {
					glBindTexture(GL_TEXTURE_2D, 0);
				}
			}

			glActiveTexture(GL_TEXTURE0);

			// --- END TEXTURE MANAGEMENT ---
		}
	}
}