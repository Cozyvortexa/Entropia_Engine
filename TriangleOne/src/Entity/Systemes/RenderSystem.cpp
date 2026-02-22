//#include "Entity/Systemes/RenderSystem.h"
//
//RenderSystem::RenderSystem(unsigned int* newFramebuffer, Camera* newMainCamera) {
//	framebuffer = newFramebuffer;
//	InitShadowMap();
//	mainCamera = newMainCamera;
//}
//
//
//
//void RenderSystem::UpdateLight(std::shared_ptr<Shader> shader, std::vector<DirLight*> directionalLightList, 
//	std::vector <std::pair<PointLight*,Transform*>> pointLightList,
//	std::vector<std::pair<SpotLight*, Transform*>> spotLightList) {
//	shader->Use();
//	for (DirLight* dirLight: directionalLightList) {
//		shader->setVec3("dirLight.direction", glm::normalize(dirLight->direction));
//		shader->setVec3("dirLight.ambient", dirLight->ambient);
//		shader->setVec3("dirLight.diffuse", dirLight->diffuse);
//		shader->setVec3("dirLight.specular", dirLight->specular);
//	}
//
//	for (int i = 0; i < pointLightList.size() ;i++) {
//		shader->setVec3("pointLights[" + std::to_string(i) + "].position", pointLightList[i].second->position);
//
//		shader->setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLightList[i].first->ambient);
//		shader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightList[i].first->diffuse);
//		shader->setVec3("pointLights[" + std::to_string(i) + "].specular", pointLightList[i].first->specular);
//
//		shader->setFloat("pointLights[" + std::to_string(i) + "].range", pointLightList[i].first->range);
//
//	}
//	for (int i = 0; i < spotLightList.size(); i++) {
//		shader->setVec3("spotLights[" + std::to_string(i) + "].position", spotLightList[i].second->position);
//
//		glm::mat4 rot(1.0f);
//
//		rot = glm::rotate(rot, glm::radians(spotLightList[i].second->rotation.x), glm::vec3(1, 0, 0));
//		rot = glm::rotate(rot, glm::radians(spotLightList[i].second->rotation.y), glm::vec3(0, 1, 0));
//		rot = glm::rotate(rot, glm::radians(spotLightList[i].second->rotation.z), glm::vec3(0, 0, 1));
//
//		shader->setVec3("spotLights[" + std::to_string(i) + "].direction", glm::normalize(glm::vec3(rot * glm::vec4(spotLightList[i].first->direction, 0.0f))));
//
//		shader->setVec3("spotLights[" + std::to_string(i) + "].ambient", spotLightList[i].first->ambient);
//		shader->setVec3("spotLights[" + std::to_string(i) + "].diffuse", spotLightList[i].first->diffuse);
//		shader->setVec3("spotLights[" + std::to_string(i) + "].specular", spotLightList[i].first->specular);
//
//		shader->setFloat("spotLights[" + std::to_string(i) + "].cutOff", glm::cos(glm::radians(spotLightList[i].first->cutOff)));
//		shader->setFloat("spotLights[" + std::to_string(i) + "].outerCutOff", glm::cos(glm::radians(spotLightList[i].first->outerCutOff)));
//		shader->setFloat("spotLights[" + std::to_string(i) + "].range", spotLightList[i].first->range);
//	}
//	int activePointLights = std::min((int)pointLightList.size(), 8); // Bloquer à 8 max
//	int activeSpotLights = std::min((int)spotLightList.size(), 8); // Bloquer à 8 max
//
//	shader->setInt("nbrPointLight", activePointLights);
//	shader->setInt("nbrSpotLight", activeSpotLights);
//}
//
//glm::mat4 RenderSystem::CalculModel(Transform* currentTransform) {
//	glm::mat4 model = _model;
//
//
//	model = glm::translate(model, currentTransform->position);
//
//	model = glm::rotate(model, glm::radians(currentTransform->rotation.x), glm::vec3(1, 0, 0));
//	model = glm::rotate(model, glm::radians(currentTransform->rotation.y), glm::vec3(0, 1, 0));
//	model = glm::rotate(model, glm::radians(currentTransform->rotation.z), glm::vec3(0, 0, 1));
//
//	model = glm::scale(model, glm::vec3(currentTransform->scale));
//
//	return model;
//}
//
//#pragma region Shadow
//void RenderSystem::InitShadowMap() {
//	glGenFramebuffers(1, &depthMapFBO);
//
//	glGenTextures(1, &depthMap);
//	glBindTexture(GL_TEXTURE_2D, depthMap);
//	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//
//	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
//
//	glDrawBuffer(GL_NONE);
//	glReadBuffer(GL_NONE);
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		std::cout << "Shadow Framebuffer not complete!" << std::endl;
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}
//
//void RenderSystem::DrawShadowForDirLight(DirLight* currentLight, Scene* scene) {  // Bug sur la window si resize
//	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//	glClear(GL_DEPTH_BUFFER_BIT);
//
//	currentLight->depthShader->Use();
//	currentLight->depthShader->setMatrix("lightSpaceMatrix", currentLight->lightMatrice);
//
//	for (const auto& currentEntity : scene->GetEntities()) {
//		if (!currentEntity->HasComponent<MeshComponent>() || !currentEntity->HasComponent<Transform>() || currentEntity->HasComponent<DirLight>()) {
//			continue;
//		}
//
//		MeshComponent* currentModel = currentEntity->GetComponent<MeshComponent>();
//		std::shared_ptr<Shader> shader = currentModel->GetShader();
//		if (currentModel->haveToBeDraw && currentModel->castShadow) {
//			currentLight->depthShader->setMatrix("model", CalculModel(currentEntity->GetComponent<Transform>()));
//			currentModel->modelMesh->DrawWithoutTexture(currentLight->depthShader);
//		}
//	}
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
//
//}
//
//void RenderSystem::DrawShadowForPointLight(std::pair<PointLight*, Transform*> currentLight, Scene* scene) {
//	currentLight.first->aspect = (float)currentLight.first->shadowWidth / (float)currentLight.first->shadowHeight;
//	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), currentLight.first->aspect, currentLight.first->near_plane, currentLight.first->range);
//
//
//	glViewport(0, 0, currentLight.first->shadowWidth, currentLight.first->shadowHeight);
//	glBindFramebuffer(GL_FRAMEBUFFER, currentLight.first->depthCubeMapFBO);  // Fbo unique par point light
//	glClear(GL_DEPTH_BUFFER_BIT);
//
//
//	std::vector<glm::mat4> shadowTransforms;
//	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
//	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
//	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
//	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
//	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
//	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
//
//
//	currentLight.first->depthShaderCubeMap->Use();
//	currentLight.first->depthShaderCubeMap->setFloat("far_plane", currentLight.first->range);
//	currentLight.first->depthShaderCubeMap->setVec3("lightPos", currentLight.second->position);
//
//
//	for (int i = 0; i < shadowTransforms.size(); i++) {
//		currentLight.first->depthShaderCubeMap->setMatrix("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
//	}
//
//	for (const auto& currentEntity : scene->GetEntities()) {
//		if (!currentEntity->HasComponent<MeshComponent>() || !currentEntity->HasComponent<Transform>()) {
//			continue;
//		}
//		MeshComponent* currentModel = currentEntity->GetComponent<MeshComponent>();
//		std::shared_ptr<Shader> shader = currentModel->GetShader();
//
//		currentLight.first->depthShaderCubeMap->setMatrix("model", CalculModel(currentEntity->GetComponent<Transform>()));
//		currentModel->modelMesh->DrawWithoutTexture(currentLight.first->depthShaderCubeMap);
//	}
//
//
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
//}
//
//void RenderSystem::DrawShadowForSpotLight(std::pair<SpotLight*, Transform*> currentLight, Scene* scene) {
//	currentLight.first->aspect = (float)currentLight.first->shadowWidth / (float)currentLight.first->shadowHeight;
//	glm::mat4 shadowProj = glm::perspective(glm::radians(currentLight.first->outerCutOff * 2.0f), currentLight.first->aspect, 0.1f, currentLight.first->range);
//
//	glm::vec3 up = (glm::abs(currentLight.first->direction.y) > 0.99f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
//	glm::mat4 shadowView = glm::lookAt(currentLight.second->position, currentLight.second->position + currentLight.first->direction, up);
//
//
//	glm::mat4 lightSpaceMatrix = shadowProj * shadowView;
//	currentLight.first->lightSpaceMatrix = lightSpaceMatrix;
//
//	glViewport(0, 0, currentLight.first->shadowWidth, currentLight.first->shadowHeight);
//	glBindFramebuffer(GL_FRAMEBUFFER, currentLight.first->depthMapFBO);  // Fbo unique par spot light
//	glClear(GL_DEPTH_BUFFER_BIT);
//
//
//	currentLight.first->depthSpotShaderMap->Use();
//	currentLight.first->depthSpotShaderMap->setMatrix("lightSpaceMatrix", lightSpaceMatrix);
//
//
//	for (const auto& currentEntity : scene->GetEntities()) {
//		if (!currentEntity->HasComponent<MeshComponent>() || !currentEntity->HasComponent<Transform>()) {
//			continue;
//		}
//
//		MeshComponent* currentModel = currentEntity->GetComponent<MeshComponent>();
//		std::shared_ptr<Shader> shader = currentModel->GetShader();
//		if (currentModel->haveToBeDraw && currentModel->castShadow) {
//			currentLight.first->depthSpotShaderMap->setMatrix("model", CalculModel(currentEntity->GetComponent<Transform>()));
//			currentModel->modelMesh->DrawWithoutTexture(currentLight.first->depthSpotShaderMap);
//		}
//	}
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
//}
//
//void RenderSystem::UpdateShadow(Scene* scene, glm::mat4 projection, std::vector <DirLight*> star, 
//	std::vector<std::pair<PointLight*, Transform*>> pointLights, 
//	std::vector<std::pair<SpotLight*, Transform*>> spotLights) {
//	glCullFace(GL_FRONT);
//	//glEnable(GL_DEPTH_CLAMP);
//	if (star.size() > 0) {
//
//		star[0]->UpdateMatrix(projection, mainCamera->GetViewMatrix());
//		DrawShadowForDirLight(star[0], scene);
//	}
//	for (const auto& currentPointLight : pointLights) {
//		DrawShadowForPointLight(currentPointLight, scene);
//	}
//	for (const auto& currentSpotLight : spotLights) {
//		DrawShadowForSpotLight(currentSpotLight, scene);
//	}
//
//	//glDisable(GL_DEPTH_CLAMP);
//	glCullFace(GL_BACK);
//}
//
//#pragma endregion Shadow
//
//
//void RenderSystem::RenderScene(Scene* scene, glm::mat4 projection) {
//	std::vector<DirLight*> star;
//	std::vector<std::pair<PointLight*, Transform*>> pointLights;
//	std::vector<std::pair<SpotLight*, Transform*>> spotLights;
//
//	for (const auto& currentEntity : scene->GetEntities()) {
//		if (currentEntity->HasComponent<DirLight>()) {
//			star.push_back(currentEntity->GetComponent<DirLight>());
//		}
//		else if (currentEntity->HasComponent<PointLight>() && currentEntity->HasComponent<Transform>()) {  // HasComponent Transform a enlever
//			pointLights.push_back(std::make_pair(currentEntity->GetComponent<PointLight>(), currentEntity->GetComponent<Transform>()));
//		}
//		else if (currentEntity->HasComponent<SpotLight>() && currentEntity->HasComponent<Transform>()) {
//			spotLights.push_back(std::make_pair(currentEntity->GetComponent<SpotLight>(), currentEntity->GetComponent<Transform>()));
//		}
//	}
//	//
//	UpdateShadow(scene, projection, star, pointLights, spotLights);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//	if (pointLights.size() >= 8) std::cout << "Max pointLight number reach" << std::endl;  // Valeur a definir a l'avenir dans un dossier config
//
//	for (const auto& currentEntity : scene->GetEntities()) {
//		if (!currentEntity->HasComponent<MeshComponent>()) {
//			continue;
//		}
//		MeshComponent* currentModel = currentEntity->GetComponent<MeshComponent>();
//		std::shared_ptr<Shader> shader = currentModel->GetShader();
//
//		if (currentModel->haveToBeDraw) {
//			shader->Use();
//
//			// --- TEXTURE UNIT MANAGEMENT ---
//
//			const int SLOT_SHADOW_DIR = 16;     
//			const int SLOT_SHADOW_POINT_START = 17; 
//			const int SLOT_SHADOW_SPOT_START = 25; 
//
//			// --- 2. GESTION LUMIERE DIRECTIONNELLE (Shadow Map 2D) ---
//			shader->setInt("shadowMap", SLOT_SHADOW_DIR);
//
//			glActiveTexture(GL_TEXTURE0 + SLOT_SHADOW_DIR);
//			if (star.size() > 0) {
//				glBindTexture(GL_TEXTURE_2D, depthMap);
//				shader->setMatrix("lightSpaceMatrix", star.at(0)->lightMatrice);
//			}
//			else {
//				glBindTexture(GL_TEXTURE_2D, 0);
//			}
//
//			// --- 3. GESTION POINT LIGHTS (Shadow Cube Maps) ---
//			int maxPointLights = 8;
//			int activeLights = std::min((int)pointLights.size(), maxPointLights);
//			shader->setInt("nbrPointLight", activeLights);
//
//			//PointLight
//			for (int i = 0; i < maxPointLights; i++) {
//				// Construction du nom "shadowCubeMaps[0]", "shadowCubeMaps[1]"...
//				std::string uniformName = "shadowCubeMaps[" + std::to_string(i) + "]";
//
//				int currentSlot = SLOT_SHADOW_POINT_START + i;
//
//				// 1. On dit au shader : "Le sampler i doit lire dans le slot X"
//				shader->setInt(uniformName, currentSlot);
//
//				// 2. On active le slot X
//				glActiveTexture(GL_TEXTURE0 + currentSlot);
//
//				if (i < activeLights) {
//					glBindTexture(GL_TEXTURE_CUBE_MAP, pointLights[i].first->depthCubeMap);
//				}
//				else {
//					// Nettoyage des slots inutilisés (évite les bugs de "Sampler Type Mismatch")
//					glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//				}
//			}
//			
//			//// Gestion Spot Light
//			int maxSpotLights = 8;
//			for (int i = 0; i < maxSpotLights; i++) {
//				std::string uniformName = "shadowMapSpot[" + std::to_string(i) + "]";
//				std::string uniformNameMatrice = "spotLightMatrices[" + std::to_string(i) + "]";
//
//				int currentSlot = SLOT_SHADOW_SPOT_START + i;
//
//				shader->setInt(uniformName, currentSlot);
//
//
//				glActiveTexture(GL_TEXTURE0 + currentSlot);
//
//
//				if (i < activeLights) {
//					shader->setMatrix(uniformNameMatrice, spotLights[i].first->lightSpaceMatrix);
//					glBindTexture(GL_TEXTURE_2D, spotLights[i].first->depthMap);
//				}
//				else {
//					glBindTexture(GL_TEXTURE_2D, 0);
//				}
//			}
//
//			glActiveTexture(GL_TEXTURE0);
//
//			// --- END TEXTURE MANAGEMENT ---
//
//			UpdateLight(shader, star, pointLights, spotLights);
//
//			shader->setMatrix("model", CalculModel(currentEntity->GetComponent<Transform>()));
//			currentModel->modelMesh->Draw(shader);
//		}
//	}
//	star.clear();
//}