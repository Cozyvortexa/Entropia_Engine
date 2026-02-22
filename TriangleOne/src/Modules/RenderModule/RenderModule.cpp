#include <Systemes/RenderModule/RenderModule.h>


void RenderModule::DrawTextureOnScreen() {

	ppShader->Use();
	glBindVertexArray(quadVAO);

	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, finalTxtOutput);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, finalTxtColorOutput);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, Window::GetWidth(), Window::GetHeight(), 0, 0, Window::GetWidth(), Window::GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void RenderModule::InitQuadVao() {
	//Init fbo
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);


	//Init texture depth
	glGenTextures(1, &finalTxtOutput);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, finalTxtOutput);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample, GL_DEPTH_COMPONENT24, Window::GetWidth(), Window::GetHeight(), GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, finalTxtOutput, 0);
	//

	//Init texture color
	glGenTextures(1, &finalTxtColorOutput);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, finalTxtColorOutput);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample, GL_RGB, Window::GetWidth(), Window::GetHeight(), GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, finalTxtColorOutput, 0);
	//

	//Assert
	if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "L'Init du quadVao a echouer" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//Init quadVAO
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	//Position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void RenderModule::InitShadowMap() {
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

#pragma region Light
void RenderModule::UpdateLight(std::shared_ptr<Shader> shader, std::vector<DirLight*> directionalLightList,
	std::vector <std::pair<PointLight*, Transform*>> pointLightList,
	std::vector<std::pair<SpotLight*, Transform*>> spotLightList) {
	shader->Use();
	for (DirLight* dirLight : directionalLightList) {
		shader->setVec3("dirLight.direction", glm::normalize(dirLight->direction));
		shader->setVec3("dirLight.ambient", dirLight->ambient);
		shader->setVec3("dirLight.diffuse", dirLight->diffuse);
		shader->setVec3("dirLight.specular", dirLight->specular);
	}

	for (int i = 0; i < pointLightList.size(); i++) {
		shader->setVec3("pointLights[" + std::to_string(i) + "].position", pointLightList[i].second->position);

		shader->setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLightList[i].first->ambient);
		shader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightList[i].first->diffuse);
		shader->setVec3("pointLights[" + std::to_string(i) + "].specular", pointLightList[i].first->specular);

		shader->setFloat("pointLights[" + std::to_string(i) + "].range", pointLightList[i].first->range);

	}
	for (int i = 0; i < spotLightList.size(); i++) {
		shader->setVec3("spotLights[" + std::to_string(i) + "].position", spotLightList[i].second->position);

		glm::mat4 rot(1.0f);

		rot = glm::rotate(rot, glm::radians(spotLightList[i].second->rotation.x), glm::vec3(1, 0, 0));
		rot = glm::rotate(rot, glm::radians(spotLightList[i].second->rotation.y), glm::vec3(0, 1, 0));
		rot = glm::rotate(rot, glm::radians(spotLightList[i].second->rotation.z), glm::vec3(0, 0, 1));

		shader->setVec3("spotLights[" + std::to_string(i) + "].direction", glm::normalize(glm::vec3(rot * glm::vec4(spotLightList[i].first->direction, 0.0f))));

		shader->setVec3("spotLights[" + std::to_string(i) + "].ambient", spotLightList[i].first->ambient);
		shader->setVec3("spotLights[" + std::to_string(i) + "].diffuse", spotLightList[i].first->diffuse);
		shader->setVec3("spotLights[" + std::to_string(i) + "].specular", spotLightList[i].first->specular);

		shader->setFloat("spotLights[" + std::to_string(i) + "].cutOff", glm::cos(glm::radians(spotLightList[i].first->cutOff)));
		shader->setFloat("spotLights[" + std::to_string(i) + "].outerCutOff", glm::cos(glm::radians(spotLightList[i].first->outerCutOff)));
		shader->setFloat("spotLights[" + std::to_string(i) + "].range", spotLightList[i].first->range);
	}
	int activePointLights = std::min((int)pointLightList.size(), 8); // Bloquer à 8 max
	int activeSpotLights = std::min((int)spotLightList.size(), 8); // Bloquer à 8 max

	shader->setInt("nbrPointLight", activePointLights);
	shader->setInt("nbrSpotLight", activeSpotLights);
}

glm::mat4 RenderModule::CalculModel(Transform* currentTransform, glm::mat4 _model) {
	glm::mat4 model = _model;


	model = glm::translate(model, currentTransform->position);

	model = glm::rotate(model, glm::radians(currentTransform->rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(currentTransform->rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(currentTransform->rotation.z), glm::vec3(0, 0, 1));

	model = glm::scale(model, glm::vec3(currentTransform->scale));

	return model;
}



#pragma endregion Light

#pragma region Shadow

void RenderModule::DrawShadowForDirLight(DirLight* currentLight, Scene* scene) {  // Bug sur la window si resize
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

void RenderModule::DrawShadowForPointLight(std::pair<PointLight*, Transform*> currentLight, Scene* scene) {
	currentLight.first->aspect = (float)currentLight.first->shadowWidth / (float)currentLight.first->shadowHeight;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), currentLight.first->aspect, currentLight.first->near_plane, currentLight.first->range);


	glViewport(0, 0, currentLight.first->shadowWidth, currentLight.first->shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, currentLight.first->depthCubeMapFBO);  // Fbo unique par point light
	glClear(GL_DEPTH_BUFFER_BIT);


	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(currentLight.second->position, currentLight.second->position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));


	currentLight.first->depthShaderCubeMap->Use();
	currentLight.first->depthShaderCubeMap->setFloat("far_plane", currentLight.first->range);
	currentLight.first->depthShaderCubeMap->setVec3("lightPos", currentLight.second->position);


	for (int i = 0; i < shadowTransforms.size(); i++) {
		currentLight.first->depthShaderCubeMap->setMatrix("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	}

	for (const auto& currentEntity : scene->GetEntities()) {
		if (!currentEntity->HasComponent<MeshComponent>() || !currentEntity->HasComponent<Transform>()) {
			continue;
		}
		MeshComponent* currentModel = currentEntity->GetComponent<MeshComponent>();
		std::shared_ptr<Shader> shader = currentModel->GetShader();

		currentLight.first->depthShaderCubeMap->setMatrix("model", CalculModel(currentEntity->GetComponent<Transform>()));
		currentModel->modelMesh->DrawWithoutTexture(currentLight.first->depthShaderCubeMap);
	}



	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
}

void RenderModule::DrawShadowForSpotLight(std::pair<SpotLight*, Transform*> currentLight, Scene* scene) {
	currentLight.first->aspect = (float)currentLight.first->shadowWidth / (float)currentLight.first->shadowHeight;
	glm::mat4 shadowProj = glm::perspective(glm::radians(currentLight.first->outerCutOff * 2.0f), currentLight.first->aspect, 0.1f, currentLight.first->range);

	glm::vec3 up = (glm::abs(currentLight.first->direction.y) > 0.99f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
	glm::mat4 shadowView = glm::lookAt(currentLight.second->position, currentLight.second->position + currentLight.first->direction, up);


	glm::mat4 lightSpaceMatrix = shadowProj * shadowView;
	currentLight.first->lightSpaceMatrix = lightSpaceMatrix;

	glViewport(0, 0, currentLight.first->shadowWidth, currentLight.first->shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, currentLight.first->depthMapFBO);  // Fbo unique par spot light
	glClear(GL_DEPTH_BUFFER_BIT);


	currentLight.first->depthSpotShaderMap->Use();
	currentLight.first->depthSpotShaderMap->setMatrix("lightSpaceMatrix", lightSpaceMatrix);


	for (const auto& currentEntity : scene->GetEntities()) {
		if (!currentEntity->HasComponent<MeshComponent>() || !currentEntity->HasComponent<Transform>()) {
			continue;
		}

		MeshComponent* currentModel = currentEntity->GetComponent<MeshComponent>();
		std::shared_ptr<Shader> shader = currentModel->GetShader();
		if (currentModel->haveToBeDraw && currentModel->castShadow) {
			currentLight.first->depthSpotShaderMap->setMatrix("model", CalculModel(currentEntity->GetComponent<Transform>()));
			currentModel->modelMesh->DrawWithoutTexture(currentLight.first->depthSpotShaderMap);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
}

void RenderModule::UpdateShadow(Scene* scene, glm::mat4 projection, std::vector <DirLight*> star,
	std::vector<std::pair<PointLight*, Transform*>> pointLights,
	std::vector<std::pair<SpotLight*, Transform*>> spotLights) {
	glCullFace(GL_FRONT);
	//glEnable(GL_DEPTH_CLAMP);
	if (star.size() > 0) {

		star[0]->UpdateMatrix(projection, mainCamera->GetViewMatrix());
		DrawShadowForDirLight(star[0], scene);
	}
	for (const auto& currentPointLight : pointLights) {
		DrawShadowForPointLight(currentPointLight, scene);
	}
	for (const auto& currentSpotLight : spotLights) {
		DrawShadowForSpotLight(currentSpotLight, scene);
	}

	//glDisable(GL_DEPTH_CLAMP);
	glCullFace(GL_BACK);
}

#pragma endregion Shadow

void RenderModule::RenderScene(Scene* scene, glm::mat4 projection) {
	std::vector<DirLight*> star;
	std::vector<std::pair<PointLight*, Transform*>> pointLights;
	std::vector<std::pair<SpotLight*, Transform*>> spotLights;

	for (const auto& currentEntity : scene->GetEntities()) {
		if (currentEntity->HasComponent<DirLight>()) {
			star.push_back(currentEntity->GetComponent<DirLight>());
		}
		else if (currentEntity->HasComponent<PointLight>() && currentEntity->HasComponent<Transform>()) {  // HasComponent Transform a enlever
			pointLights.push_back(std::make_pair(currentEntity->GetComponent<PointLight>(), currentEntity->GetComponent<Transform>()));
		}
		else if (currentEntity->HasComponent<SpotLight>() && currentEntity->HasComponent<Transform>()) {
			spotLights.push_back(std::make_pair(currentEntity->GetComponent<SpotLight>(), currentEntity->GetComponent<Transform>()));
		}
	}
	//
	UpdateShadow(scene, projection, star, pointLights, spotLights);

	glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (pointLights.size() >= 8) std::cout << "Max pointLight number reach" << std::endl;  // Valeur a definir a l'avenir dans un dossier config

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
			if (star.size() > 0) {
				glBindTexture(GL_TEXTURE_2D, depthMap);
				shader->setMatrix("lightSpaceMatrix", star.at(0)->lightMatrice);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			// --- 3. GESTION POINT LIGHTS (Shadow Cube Maps) ---
			int maxPointLights = 8;
			int activeLights = std::min((int)pointLights.size(), maxPointLights);
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
					glBindTexture(GL_TEXTURE_CUBE_MAP, pointLights[i].first->depthCubeMap);
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

			UpdateLight(shader, star, pointLights, spotLights);

			shader->setMatrix("model", CalculModel(currentEntity->GetComponent<Transform>()));
			currentModel->modelMesh->Draw(shader);
		}
	}
	star.clear();
}

void RenderModule::Init(World& world) {
	GLFWwindow* window = world.get_ressource<WindowRessource>()->window;
	RenderRessource* ressource = world.get_ressource<RenderRessource>();

	if (window == nullptr) {
		std::cout << "Reference de la window impossible a recuperer" << std::endl;
		abort();
	}

	Shader::CreateDefaultWhiteTexture();
	ressource->mainShader = std::make_unique<Shader>("TriangleOne/Shader/MainShader/BaseVertexShader.glsl", "TriangleOne/Shader/MainShader/BaseFragmentShader.glsl");
	ressource->depthShader = std::make_unique<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/DepthMapVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/DepthMapFrag.glsl");
	ressource->depthShaderCubeMap = std::make_unique<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeFrag.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeGeometry.glsl");



	renderSystem = new RenderSystem(&framebuffer, mainCamera);




















	currentScene = new Scene();
	//Maison
	Entity* entity = currentScene->CreateNewEntity();
	entity->AddComponent<Transform>();
	entity->AddComponent<MeshComponent>("Assets/ImpScene/autumn_house.glb", mainShader);
	//std::shared_ptr<MeshComponent> meshAttachToEntity = currentScene->AddComponent<MeshComponent>(entity, "Assets/ImpScene/autumn_house.glb", mainShader);



	glm::vec3 ambient = glm::vec3(0.002f, 0.002f, 0.002f);
	glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 specular = glm::vec3(0.0f, 0.0f, 0.0f);

	float intensity = 3.0f;

	glm::vec3 worldLightDir = glm::normalize(glm::vec3(-2.0f, 4.0f, -1.0f));

	//DirLight   	//	DirLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, std::shared_ptr<Shader> _depthShader)
	Entity* entityLight = currentScene->CreateNewEntity();
	entityLight->AddComponent<Transform>();
	entityLight->AddComponent<DirLight>(glm::vec3(0.002f, 0.002f, 0.002f), diffuse, specular, worldLightDir, depthShader, intensity);

	float pointLightrange = 8.0f;


	//PointLight
	Entity* entityPointLight = currentScene->CreateNewEntity();
	entityPointLight->AddComponent<Transform>()->position = glm::vec3(1.0f, 3.0f, 0.0f);
	entityPointLight->AddComponent<PointLight>(ambient, diffuse, specular, pointLightrange, depthShaderCubeMap, 5.0f);

	//SpotLight
	float cutOff = 15.5f;
	float outerCutOff = 25.5f;
	Entity* entitySpotLight = currentScene->CreateNewEntity();
	entitySpotLight->AddComponent<Transform>()->position = glm::vec3(0.0f, 4.0f, -6.0f);
	entitySpotLight->AddComponent<SpotLight>(ambient, diffuse, specular, glm::vec3(1.0f, 0.0f, 0.0f), cutOff, outerCutOff, 30.0f, depthShader, 10.0f);  // shader identique a celui de la dirLight

	Entity* cubeTest = currentScene->CreateNewEntity();
	cubeTest->AddComponent<Transform>()->position = glm::vec3(4.0f, 0.0f, 0.0f);
	cubeTest->AddComponent<MeshComponent>("Assets/ImpScene/BasicCube.glb", mainShader);



	//	SpotLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, float _cutOff, float _outercutOff, float range, float newIntensity)
	//	PointLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadratique, std::shared_ptr<Shader> _depthShaderCubeMap) 


	//dirLight = new Light(glm::vec3(0), worldLightDir, ambient, diffuse, specular);


	InitQuadVao();
	//InitSkyBox();
}

void RenderModule::Update(World& world)
 {
	WindowRessource* windowRessource = world.get_ressource<WindowRessource>();
	Entity mainCam = world.get_ressource<ActiveCamera>()->cameraID;

	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->GetZoom()), (float)windowRessource->WIDHT / (float)windowRessource->HEIGHT, mainCamera->GetNearPlane(), mainCamera->GetFarPlane());

	mainShader->Use(); 
	mainShader->setMatrix("model", _model);
	mainShader->setMatrix("view", mainCamera->GetViewMatrix());
	mainShader->setMatrix("projection", projection);
	mainShader->setFloat("far_plane", mainCamera->GetFarPlane());

	mainShader->setVec3("viewPos", mainCamera->GetPos());


	RenderScene(currentScene, projection);


	//DrawSkyBox(projection);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawTextureOnScreen();

	// A deplacer dans un input manager
	mainCamera->ProcessInput(window);
	
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void RenderModule::Shutdown(World& world) {
	std::cout << "Shuting down RenderModule" << std::endl;
}
