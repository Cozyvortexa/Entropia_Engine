#include <Systems/RenderSystem.h>


void RenderSystem::DrawTextureOnScreen(WindowResource* windowData, RenderResource* renderData) {

	glBindFramebuffer(GL_READ_FRAMEBUFFER, renderData->framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderData->intermediateFBO);

	//Color image
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, windowData->WIDTH, windowData->HEIGHT, 0, 0, windowData->WIDTH, windowData->HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	//Bloom texture
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, windowData->WIDTH, windowData->HEIGHT, 0, 0, windowData->WIDTH, windowData->HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	if (renderData->bloomEnable) {
		DrawBlurEffect(renderData);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);


	//Post process
	renderData->postProcessShader->Use();
	glBindVertexArray(renderData->quadVAO);
	//Parameters
	renderData->postProcessShader->setFloat("exposure", renderData->exposure);
	glDisable(GL_DEPTH_TEST);

	//Scene image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderData->screenTexture);
	renderData->postProcessShader->setInt("scene", 0);

	//Bloom image
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, renderData->pingpongBuffers[!renderData->horizontal]); // Le dernier buffer utilisé
	renderData->postProcessShader->setInt("bloomBlur", 1);
	renderData->postProcessShader->setBool("bloomEnable", renderData->bloomEnable);

	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtColorOutput);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void RenderSystem::DrawBlurEffect(RenderResource* renderData) {
	bool first_iteration = true;
	int amount = renderData->bloom_iteration;

	renderData->bloomShader->Use();
	glBindVertexArray(renderData->quadVAO);
	glDisable(GL_DEPTH_TEST);
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderData->pingpongFBO[renderData->horizontal]);
		renderData->bloomShader->setBool("horizontal", renderData->horizontal);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? renderData->bloomTextureResolved : renderData->pingpongBuffers[!renderData->horizontal]);

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

void RenderSystem::InitMainFrameBuffer(WindowResource* windowData, RenderResource* renderData) {
	///////////////////Init fbo
	glGenFramebuffers(1, &renderData->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->framebuffer);


	//////////////////Init texture depth
	glGenTextures(1, &renderData->finalTxtOutput);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtOutput);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderData->sample, GL_DEPTH_COMPONENT24, windowData->WIDTH, windowData->HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtOutput, 0);


	//////////////////Init texture color [0] and Bloom texture [1] 
	glGenTextures(2, renderData->finalTxtColorOutput);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtColorOutput[i]);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderData->sample, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, renderData->finalTxtColorOutput[i], 0);
	}
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);


	//////////////////Assert
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout <<" The main frame buffer initialisation has failed " << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void RenderSystem::InitQuadVao(WindowResource* windowData, RenderResource* renderData) {
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

void RenderSystem::InitIntermediateFBO(WindowResource* windowData, RenderResource* renderData) {
	//Init Intermediate FBO, To resolve the MSAA
	glGenFramebuffers(1, &renderData->intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->intermediateFBO);

	glGenTextures(1, &renderData->screenTexture);
	glBindTexture(GL_TEXTURE_2D, renderData->screenTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderData->screenTexture, 0);

	glGenTextures(1, &renderData->bloomTextureResolved);
	glBindTexture(GL_TEXTURE_2D, renderData->bloomTextureResolved);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderData->bloomTextureResolved, 0);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);



	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "The intermediate FBO initialisation has failed" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::InitBloomFBO(WindowResource* windowData, RenderResource* renderData) {
	glGenFramebuffers(2, renderData->pingpongFBO);
	glGenTextures(2, renderData->pingpongBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderData->pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, renderData->pingpongBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0,GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, renderData->pingpongBuffers[i], 0);
	}
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


	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->zoom), (float)windowData->WIDTH / (float)windowData->HEIGHT, mainCamera->nearPlane, mainCamera->farPlane);


	View view = world.view<MeshHandle, Transform, MaterialHandle>();
	view.each([&](int entity, MeshHandle& meshHandle, Transform& transform, MaterialHandle& materialHandle) {
		if (meshHandle.haveToBeDraw) {
			Shader currentShader = world.assetStore->Get_Material(materialHandle.index)->shader;
			Mesh currentMesh = world.assetStore->Get_Mesh(meshHandle.index);


			currentShader.Use();

			// --- Link Matrices ---
			currentShader.setMatrix("view", mainCamera->viewMatrice);
			currentShader.setMatrix("projection", projection);
			currentShader.setFloat("far_plane", mainCamera->farPlane);
			currentShader.setVec3("viewPos", transformMainCamera->position);


			currentShader.setMatrix("model", transform.GetTransformModel());
			world.renderer->DrawMesh(currentMesh);
		}
	});
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	renderData->bloomShader = std::make_unique<Shader>("TriangleOne/Shader/BloomShader/VertexBloom.glsl", "TriangleOne/Shader/BloomShader/FragmentBloom.glsl");

	//Create the main cam  // TEMP / WARNING
	Entity camEntity = world.Register();
	CameraComponent cameraComponent(windowData->WIDTH, windowData->HEIGHT);
	Transform transform;
	world.add_components(camEntity, cameraComponent, transform);

	world.get_ressource<ActiveCamera>()->cameraID = camEntity;

	Entity model = world.Register();
	Transform modelTransform; // ("Assets/main_sponza/main_sponza/NewSponza_Main_Yup_003.fbx");
	std::pair<Mesh&, int> value = world.assetStore->Get_Mesh("Assets/ImpScene/autumn_house.glb");
	MeshHandle meshHandle(value.second);
	SceneTag sceneTag;
	MaterialHandle materialHandle(renderData->mainMaterialHandle);

	world.add_components(model, sceneTag, materialHandle, meshHandle, modelTransform);


	glm::vec3 ambient = glm::vec3(0.002f, 0.002f, 0.002f);
	glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 specular = glm::vec3(0.0f, 0.0f, 0.0f);

	float intensity = 1.0f;

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
	SpotLight spotLight(ambient, diffuse, specular, glm::vec3(1.0f, 0.0f, 0.0f), cutOff, outerCutOff, 30.0f, renderData->depthShader.get(), 20.0f);
	LightToInitTag spotTag(LightTag::SpotLight_Tag);

	world.add_components(spotLightEntity, spotTransform, spotLight, spotTag);


	/////////////////////////////////////

	Entity pointLightEntity = world.Register();
	Transform transformPointLight(glm::vec3(1.0f, 5.0f, 0.0f));
	PointLight pointLight(ambient, diffuse, specular, 8.0f, renderData->depthShaderCubeMap.get(), 40.0f);
	LightToInitTag pointTag(LightTag::PointLight_Tag);

	world.add_components(pointLightEntity, transformPointLight, pointLight, pointTag);

	/////////////////////////////////////

	Entity backpack = world.Register();
	Transform backPackTransform(glm::vec3(10.0f, 3.0f, 2.0f));
	std::pair<Mesh&, int> backpackValue = world.assetStore->Get_Mesh("Assets/backpack/backpack.obj");
	MeshHandle backpackModeleHandle(backpackValue.second);

	world.add_components(backpack, backPackTransform, sceneTag, materialHandle, backpackModeleHandle);


	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);


	InitMainFrameBuffer(windowData, renderData);
	InitIntermediateFBO(windowData, renderData);
	InitBloomFBO(windowData, renderData);
	InitQuadVao(windowData, renderData);
}

void RenderSystem::Update(World& world, const ResourceBuffer* resourceBuffer)
 {
	WindowResource* windowData = resourceBuffer->windowResource;
	RenderResource* renderData = resourceBuffer->renderResource;


	RenderScene(world, resourceBuffer, windowData);


	DrawTextureOnScreen(windowData, renderData);

	
	glfwSwapBuffers(windowData->window);
	glfwPollEvents();
}

void RenderSystem::Shutdown(World& world) {
	std::cout << "Shuting down RenderSystem" << std::endl;
}
