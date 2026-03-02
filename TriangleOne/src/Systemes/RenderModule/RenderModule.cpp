#include <Systemes/RenderModule/RenderModule.h>


void RenderModule::DrawTextureOnScreen(WindowResource* windowData, RenderResource* renderData) {

	renderData->postProcessShader->Use();
	glBindVertexArray(renderData->quadVAO);

	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtOutput);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtColorOutput);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	glBindFramebuffer(GL_READ_FRAMEBUFFER, renderData->framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, windowData->WIDHT, windowData->HEIGHT, 0, 0, windowData->WIDHT, windowData->HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void RenderModule::InitQuadVao(WindowResource* windowData, RenderResource* renderData) {
	//Init fbo
	glGenFramebuffers(1, &renderData->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->framebuffer);


	//Init texture depth
	glGenTextures(1, &renderData->finalTxtOutput);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtOutput);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderData->sample, GL_DEPTH_COMPONENT24, windowData->WIDHT, windowData->HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtOutput, 0);
	//

	//Init texture color
	glGenTextures(1, &renderData->finalTxtColorOutput);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtColorOutput);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderData->sample, GL_RGB, windowData->WIDHT, windowData->HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtColorOutput, 0);
	//

	//Assert
	if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "L'Init du quadVao a echouer" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//Init quadVAO
	glGenVertexArrays(1, &renderData->quadVAO);
	glBindVertexArray(renderData->quadVAO);

	glGenBuffers(1, &renderData->quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, renderData->quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(renderData->quadVertices), renderData->quadVertices, GL_STATIC_DRAW);

	//Position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
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


#pragma endregion Light



void RenderModule::RenderScene(World& world, const ResourceBuffer* resourceBuffer, CameraComponent* mainCamera, glm::mat4 projection) {
	glBindFramebuffer(GL_FRAMEBUFFER, resourceBuffer->renderResource->framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


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
				glBindTexture(GL_TEXTURE_2D, renderData->depthMap);
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
}

void RenderModule::Init(World& world) {
	WindowResource* windowData = world.get_ressource<WindowResource>();
	RenderResource* renderData = world.get_ressource<RenderResource>();

	if (windowData->window == nullptr) {
		std::cout << "Reference de la window impossible a recuperer" << std::endl;
		abort();
	}

	Shader::CreateDefaultWhiteTexture();
	renderData->mainShader = std::make_unique<Shader>("TriangleOne/Shader/MainShader/BaseVertexShader.glsl", "TriangleOne/Shader/MainShader/BaseFragmentShader.glsl");
	renderData->depthShader = std::make_unique<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/DepthMapVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/DepthMapFrag.glsl");
	renderData->depthShaderCubeMap = std::make_unique<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeFrag.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeGeometry.glsl");
	renderData->postProcessShader = std::make_unique<Shader>("TriangleOne/Shader/PostProcessShader/PostProcessVertex.glsl", "TriangleOne/Shader/PostProcessShader/PostProcessFrag.glsl");



	//currentScene = new Scene();
	////Maison
	//Entity* entity = currentScene->CreateNewEntity();
	//entity->AddComponent<Transform>();
	//entity->AddComponent<MeshComponent>("Assets/ImpScene/autumn_house.glb", mainShader);
	////std::shared_ptr<MeshComponent> meshAttachToEntity = currentScene->AddComponent<MeshComponent>(entity, "Assets/ImpScene/autumn_house.glb", mainShader);



	//glm::vec3 ambient = glm::vec3(0.002f, 0.002f, 0.002f);
	//glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	//glm::vec3 specular = glm::vec3(0.0f, 0.0f, 0.0f);

	//float intensity = 3.0f;

	//glm::vec3 worldLightDir = glm::normalize(glm::vec3(-2.0f, 4.0f, -1.0f));

	////DirLight   	//	DirLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, std::shared_ptr<Shader> _depthShader)
	//Entity* entityLight = currentScene->CreateNewEntity();
	//entityLight->AddComponent<Transform>();
	//entityLight->AddComponent<DirLight>(glm::vec3(0.002f, 0.002f, 0.002f), diffuse, specular, worldLightDir, depthShader, intensity);

	//float pointLightrange = 8.0f;


	////PointLight
	//Entity* entityPointLight = currentScene->CreateNewEntity();
	//entityPointLight->AddComponent<Transform>()->position = glm::vec3(1.0f, 3.0f, 0.0f);
	//entityPointLight->AddComponent<PointLight>(ambient, diffuse, specular, pointLightrange, depthShaderCubeMap, 5.0f);

	////SpotLight
	//float cutOff = 15.5f;
	//float outerCutOff = 25.5f;
	//Entity* entitySpotLight = currentScene->CreateNewEntity();
	//entitySpotLight->AddComponent<Transform>()->position = glm::vec3(0.0f, 4.0f, -6.0f);
	//entitySpotLight->AddComponent<SpotLight>(ambient, diffuse, specular, glm::vec3(1.0f, 0.0f, 0.0f), cutOff, outerCutOff, 30.0f, depthShader, 10.0f);  // shader identique a celui de la dirLight

	//Entity* cubeTest = currentScene->CreateNewEntity();
	//cubeTest->AddComponent<Transform>()->position = glm::vec3(4.0f, 0.0f, 0.0f);
	//cubeTest->AddComponent<MeshComponent>("Assets/ImpScene/BasicCube.glb", mainShader);



	//	SpotLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, float _cutOff, float _outercutOff, float range, float newIntensity)
	//	PointLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadratique, std::shared_ptr<Shader> _depthShaderCubeMap) 


	//dirLight = new Light(glm::vec3(0), worldLightDir, ambient, diffuse, specular);


	InitQuadVao(windowData, renderData);
	//InitSkyBox();
}

void RenderModule::Update(World& world, const ResourceBuffer* resourceBuffer)
 {
	WindowResource* windowData = resourceBuffer->windowResource;
	Entity entityCam = resourceBuffer->activeCamera->cameraID;
	RenderResource* renderData = resourceBuffer->renderResource;

	CameraComponent* mainCamera = world.get_component<CameraComponent>(entityCam);  // Une vue n'est pas possible dans ce contexte car c'est une caméra spécifique qui est récupérer
	Transform* transformMainCamera = world.get_component<Transform>(entityCam);

	if (mainCamera == nullptr || transformMainCamera == nullptr) {  // Pas de main camera, pas de rendu
		std::cout << "Main camera have a null value" << std::endl;
		return;
	}

	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->zoom), (float)windowData->WIDHT / (float)windowData->HEIGHT, mainCamera->nearPlane, mainCamera->farPlane);

	renderData->mainShader->Use();
	renderData->mainShader->setMatrix("model", renderData->_model);
	renderData->mainShader->setMatrix("view", mainCamera->viewMatrice);
	renderData->mainShader->setMatrix("projection", projection);
	renderData->mainShader->setFloat("far_plane", mainCamera->farPlane);

	renderData->mainShader->setVec3("viewPos", transformMainCamera->position);


	RenderScene(world, resourceBuffer, mainCamera, projection);


	//DrawSkyBox(projection);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawTextureOnScreen(windowData, renderData);

	// A deplacer dans un input manager
	//mainCamera->ProcessInput(windowRessource->window);
	
	glfwSwapBuffers(windowData->window);
	glfwPollEvents();
}

void RenderModule::Shutdown(World& world) {
	std::cout << "Shuting down RenderModule" << std::endl;
}
