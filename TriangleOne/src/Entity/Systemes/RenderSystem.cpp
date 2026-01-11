#include "Entity/Systemes/RenderSystem.h"

RenderSystem::RenderSystem(unsigned int* newFramebuffer, Camera* newMainCamera) {
	framebuffer = newFramebuffer;
	InitShadowMap();
	mainCamera = newMainCamera;
}



void RenderSystem::UpdateLight(std::shared_ptr<Shader> shader, std::vector<DirLight*> directionalLightList) {
	shader->Use();
	for (DirLight* dirLight: directionalLightList) {
		shader->setVec3("dirLight.direction", glm::normalize(dirLight->direction));
		shader->setVec3("dirLight.ambient", dirLight->ambient);
		shader->setVec3("dirLight.diffuse", dirLight->diffuse);
		shader->setVec3("dirLight.specular", dirLight->specular);
	}

	//for (int i = 0; i < pointLightList.size() ;i++) {
	//	shader->setVec3("pointLights[" + std::to_string(i) + "].position", pointLightList[i]->position);

	//	shader->setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLightList[i]->ambient);
	//	shader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightList[i]->diffuse);
	//	shader->setVec3("pointLights[" + std::to_string(i) + "].specular", pointLightList[i]->specular);

	//	shader->setFloat("pointLights[" + std::to_string(i) + "].constant", pointLightList[i]->constant);
	//	shader->setFloat("pointLights[" + std::to_string(i) + "].linear", pointLightList[i]->linear);
	//	shader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", pointLightList[i]->quadratique);

	//}

	//for (int i = 0; i < spotLightList.size() ;i++)
	//{
	//	shader->setVec3("spotLight.Position", spotLightList[i]->position);

	//	shader->setVec3("spotLight.ambient", spotLightList[i]->ambient);
	//	shader->setVec3("spotLight.diffuse", spotLightList[i]->diffuse);
	//	shader->setVec3("spotLight.specular", spotLightList[i]->specular);

	//	shader->setFloat("spotLight.constant", spotLightList[i]->constant);
	//	shader->setFloat("spotLight.linear", spotLightList[i]->linear);
	//	shader->setFloat("spotLight.quadratic", spotLightList[i]->quadratique);

	//	shader->setVec3("spotLight.direction", spotLightList[i]->direction);
	//	shader->setFloat("spotLight.cutOff", glm::radians(spotLightList[i]->cutOff));
	//	shader->setFloat("spotLight.outerCutOff", glm::radians(spotLightList[i]->outerCutOff));

	//}

}

glm::mat4 RenderSystem::CalculModel(Transform* currentTransform) {
	glm::mat4 model = _model;


	model = glm::translate(model, currentTransform->position);

	model = glm::rotate(model, glm::radians(currentTransform->rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(currentTransform->rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(currentTransform->rotation.z), glm::vec3(0, 0, 1));

	model = glm::scale(model, glm::vec3(currentTransform->scale));

	return model;
}

#pragma region Shadow
void RenderSystem::InitShadowMap() {
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Shadow Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::DrawShadowForDirLight(DirLight* currentLight, Scene* scene) {  // Bug sur la window si resize
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	currentLight->depthShader->Use();
	currentLight->depthShader->setMatrix("lightSpaceMatrix", currentLight->lightMatrice);

	for (const auto& currentEntity : scene->GetEntities()) {
		if (!currentEntity->HasComponent<MeshComponent>() || !currentEntity->HasComponent<Transform>() || currentEntity->HasComponent<DirLight>()) {
			continue;
		}

		MeshComponent* currentModel = currentEntity->GetComponent<MeshComponent>();
		std::shared_ptr<Shader> shader = currentModel->GetShader();
		if (currentModel->haveToBeDraw && currentModel->castShadow) {
			currentLight->depthShader->setMatrix("model", CalculModel(currentEntity->GetComponent<Transform>()));
			currentModel->modelMesh->DrawWithoutTexture(currentLight->depthShader);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::GetWidth(), Window::GetHeight());

}

void RenderSystem::DrawShadowForPointLight(std::shared_ptr<PointLight> currentLight, Scene* scene) {
	currentLight->aspect = (float)currentLight->shadowWidth / (float)currentLight->shadowHeight;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), currentLight->aspect, currentLight->near_plane, currentLight->far_plane);


	glViewport(0, 0, currentLight->shadowWidth, currentLight->shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, currentLight->depthCubeMapFBO);  // Fbo unique par point light
	glClear(GL_DEPTH_BUFFER_BIT);


	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight->position, currentLight->position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight->position, currentLight->position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight->position, currentLight->position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight->position, currentLight->position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight->position, currentLight->position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight->position, currentLight->position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));


	currentLight->depthShaderCubeMap->Use();
	currentLight->depthShaderCubeMap->setFloat("far_plane", currentLight->far_plane);
	currentLight->depthShaderCubeMap->setVec3("lightPos", currentLight->position);


	for (int i = 0; i < shadowTransforms.size(); i++) {
		currentLight->depthShaderCubeMap->setMatrix("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	}

	for (const auto& currentEntity : scene->GetEntities()) {
		if (!currentEntity->HasComponent<MeshComponent>() || !currentEntity->HasComponent<Transform>()) {
			continue;
		}
		MeshComponent* currentModel = currentEntity->GetComponent<MeshComponent>();
		std::shared_ptr<Shader> shader = currentModel->GetShader();

		currentLight->depthShaderCubeMap->setMatrix("model", CalculModel(currentEntity->GetComponent<Transform>()));
		currentModel->modelMesh->DrawWithoutTexture(currentLight->depthShaderCubeMap);
	}



	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
}

void RenderSystem::UpdateShadow(Scene* scene, glm::mat4 projection) {
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_CLAMP);
	for (const auto& currentEntity : scene->GetEntities()) {
		if (currentEntity->HasComponent<DirLight>()) {
			DirLight* currentLight = currentEntity->GetComponent<DirLight>();

			currentLight->UpdateMatrix(projection, mainCamera->GetViewMatrix());
			DrawShadowForDirLight(currentLight, scene);
		}
	}

	//for (std::shared_ptr<PointLight> pointLight : pointLightList) {
	//	DrawShadowForPointLight(pointLight, scene);
	//}
	glDisable(GL_DEPTH_CLAMP);
	glCullFace(GL_BACK);
}

#pragma endregion Shadow



void RenderSystem::RenderScene(Scene* scene, glm::mat4 projection) {
	UpdateShadow(scene, projection);

	glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	std::vector<DirLight*> star;
	for (const auto& currentEntity : scene->GetEntities()) {
		if (currentEntity->HasComponent<DirLight>()) {
			star.push_back(currentEntity->GetComponent<DirLight>());
		}
	}
	for (const auto& currentEntity: scene->GetEntities()) {
		if (!currentEntity->HasComponent<MeshComponent>()) {
			continue;
		}
		MeshComponent* currentModel = currentEntity->GetComponent<MeshComponent>();
		std::shared_ptr<Shader> shader = currentModel->GetShader();

		if (currentModel->haveToBeDraw) {
			shader->Use();

			if (star.size() != 0) {
				shader->setMatrix("lightSpaceMatrix", star.at(0)->lightMatrice);
			}


			// V�rifie la position finale (colonne 3 de la matrice)

			////Link shadowMap
			//temp
			shader->setInt("shadowMap", 30);
			//shader->setInt("shadowCubeMap", 31);

			if (star.size() != 0) {
				glActiveTexture(GL_TEXTURE30);
				glBindTexture(GL_TEXTURE_2D, depthMap);
			}
			//if (pointLightList.size() != 0) {
			//	glActiveTexture(GL_TEXTURE31);
			//	glBindTexture(GL_TEXTURE_CUBE_MAP, pointLightList[0]->depthCubeMap);  // Utilise samplerCubeArray dans le shader
			//	std::cout << "enter On pointLigList machin chouette, bref viens voir par ici" << std::endl;
			//}

			
			UpdateLight(shader, star);  // Oui je sais, les light sont recalculer pour chaque modele

			currentModel->modelMesh->Draw(shader);

		}

	}
	star.clear();
}