#include <Systems/RenderSystem.h>


void RenderSystem::DrawTextureOnScreen(WindowResource* windowData, RenderResource* renderData) {

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

void RenderSystem::InitQuadVao(WindowResource* windowData, RenderResource* renderData) {
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

void RenderSystem::RenderScene(World& world, const ResourceBuffer* resourceBuffer, WindowResource* windowData) {
	glBindFramebuffer(GL_FRAMEBUFFER, resourceBuffer->renderResource->framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/////////////////////Camera
	Entity entityCam = resourceBuffer->activeCamera->cameraID;
	CameraComponent* mainCamera = world.get_component<CameraComponent>(entityCam);

	Transform* transformMainCamera = world.get_component<Transform>(entityCam);
	if (mainCamera == nullptr || transformMainCamera == nullptr) {  // Pas de main camera, pas de rendu
		std::cout << "Main camera have a null value" << std::endl;
		return;
	}
	///////////////////


	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->zoom), (float)windowData->WIDHT / (float)windowData->HEIGHT, mainCamera->nearPlane, mainCamera->farPlane);


	View view = world.view<ModeleHandle, Transform, MaterialHandle>();
	view.each([&](int entity, ModeleHandle& modeleHandle, Transform& transform, MaterialHandle& materialHandle) {
		if (modeleHandle.haveToBeDraw) {
			Shader currentShader = world.modelStore->Get_Material(materialHandle.index).shader;
			Model currentModel = world.modelStore->Get_Model(modeleHandle.index);


			currentShader.Use();

			// --- Link Matrices ---
			currentShader.setMatrix("view", mainCamera->viewMatrice);
			currentShader.setMatrix("projection", projection);
			currentShader.setFloat("far_plane", mainCamera->farPlane);
			currentShader.setVec3("viewPos", transformMainCamera->position);


			currentShader.setMatrix("model", transform.GetTransformModel());
			currentModel.Draw(&currentShader);
		}
	});
}

void RenderSystem::Init(World& world, const ResourceBuffer* resourceBuffer) {
	WindowResource* windowData = world.get_ressource<WindowResource>();
	RenderResource* renderData = world.get_ressource<RenderResource>();

	if (windowData->window == nullptr) {
		std::cout << "Reference de la window impossible a recuperer" << std::endl;
		abort();
	}

	// Main shader is created in the init of the lightSystem 
	renderData->depthShader = std::make_unique<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/DepthMapVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/DepthMapFrag.glsl");
	renderData->depthShaderCubeMap = std::make_unique<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeFrag.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeGeometry.glsl");
	renderData->postProcessShader = std::make_unique<Shader>("TriangleOne/Shader/PostProcessShader/PostProcessVertex.glsl", "TriangleOne/Shader/PostProcessShader/PostProcessFrag.glsl");

	//Create the main cam  // TEMP / WARNING
	Entity camEntity = world.Register();
	CameraComponent cameraComponent(windowData->WIDHT, windowData->HEIGHT);
	Transform transform;
	world.add_components(camEntity, cameraComponent, transform);

	world.get_ressource<ActiveCamera>()->cameraID = camEntity;

	Entity model = world.Register();
	Transform modelTransform; // ("Assets/main_sponza/main_sponza/NewSponza_Main_Yup_003.fbx");
	std::pair<Model&, int> value = world.modelStore->Get_Model("Assets/ImpScene/autumn_house.glb");
	ModeleHandle modeleHandle(value.second);
	SceneTag sceneTag;
	MaterialHandle materialHandle(renderData->mainMaterialHandle);

	world.add_components(model, sceneTag, materialHandle, modeleHandle, modelTransform);


	glm::vec3 ambient = glm::vec3(0.002f, 0.002f, 0.002f);
	glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 specular = glm::vec3(0.0f, 0.0f, 0.0f);

	float intensity = 3.0f;

	glm::vec3 worldLightDir = glm::normalize(glm::vec3(-2.0f, 4.0f, -1.0f));

	float cutOff = 5.5f;
	float outerCutOff = 15.5f;

	Entity dirLight_E = world.Register();
	DirLight dirLight(ambient, diffuse, specular, worldLightDir, renderData->depthShader.get(), intensity);

	LightToInitTag tag(LightTag::Directional_Tag);
	Transform lightTransform(glm::vec3(0.0f, 4.0f, -6.0f));

	world.add_components(dirLight_E, transform, dirLight, tag);

	/////////////////////////////////////

	Entity spotLightEntity = world.Register();
	Transform spotTransform(glm::vec3(0.0f, 4.0f, -6.0f));
	SpotLight spotLight(ambient, diffuse, specular, glm::vec3(1.0f, 0.0f, 0.0f), cutOff, outerCutOff, 30.0f, renderData->depthShader.get(), 10.0f);
	LightToInitTag spotTag(LightTag::SpotLight_Tag);

	world.add_components(spotLightEntity, spotTransform, spotLight, spotTag);


	/////////////////////////////////////

	Entity pointLightEntity = world.Register();
	Transform transformPointLight(glm::vec3(1.0f, 3.0f, 0.0f));
	PointLight pointLight(ambient, diffuse, specular, 8.0f, renderData->depthShaderCubeMap.get(), 40.0f);
	LightToInitTag pointTag(LightTag::PointLight_Tag);

	world.add_components(pointLightEntity, transformPointLight, pointLight, pointTag);

	/////////////////////////////////////

	Entity backpack = world.Register();
	Transform backPackTransform(glm::vec3(10.0f, 3.0f, 2.0f));
	std::pair<Model&, int> backpackValue = world.modelStore->Get_Model("Assets/backpack/backpack.obj");
	ModeleHandle backpackModeleHandle(backpackValue.second);

	world.add_components(backpack, backPackTransform, sceneTag, materialHandle, backpackModeleHandle);



	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);




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

void RenderSystem::Update(World& world, const ResourceBuffer* resourceBuffer)
 {
	WindowResource* windowData = resourceBuffer->windowResource;
	RenderResource* renderData = resourceBuffer->renderResource;


	RenderScene(world, resourceBuffer, windowData);


	//DrawSkyBox(projection);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawTextureOnScreen(windowData, renderData);

	// A deplacer dans un input manager
	//mainCamera->ProcessInput(windowRessource->window);
	
	glfwSwapBuffers(windowData->window);
	glfwPollEvents();
}

void RenderSystem::Shutdown(World& world) {
	std::cout << "Shuting down RenderSystem" << std::endl;
}
