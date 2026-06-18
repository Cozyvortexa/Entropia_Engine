#include <Systems/RenderSystem.h>

namespace Resource = Engine::Resource;
namespace Component = Engine::Component;
namespace Systems = Engine::Systems;

void Systems::RenderSystem::SSAO_Pass(World& world, Resource::RenderResource* renderData, Component::CameraComponent* mainCamera) {
	///////////////SSAO
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->ssao.ssaoBuffer);

	Shader* ssaoPass_Shader = renderData->r_Shader.ssaoPass_Shader.get();

	ssaoPass_Shader->Use();
	ssaoPass_Shader->setMatrix("projection", renderData->projection);
	ssaoPass_Shader->setMatrix("view", mainCamera->viewMatrice);
	ssaoPass_Shader->setFloat("radius", renderData->ssao.SSAO_radius);
	ssaoPass_Shader->setMatrix("invProjection", glm::inverse( renderData->projection));
	ssaoPass_Shader->setVec("samples", renderData->ssao.ssaoKernel);  
	ssaoPass_Shader->setInt("kernelNbr", renderData->ssao.kernelSample);

	ssaoPass_Shader->setVec("noiseScale", glm::vec2(renderData->renderWIDTH / 4.0f,  renderData->renderHEIGHT / 4.0f));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gNormal);
	ssaoPass_Shader->setInt("gNormal", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, renderData->ssao.ssao_NoiseText);
	ssaoPass_Shader->setInt("texNoise", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gDepth);
	ssaoPass_Shader->setInt("gDepthMap", 2);

	world.renderer->DrawQuad(renderData);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	///////////////Blur
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->ssao.ssaoBlur_Buffer);
	renderData->r_Shader.ssaoPass_Blur_Shader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderData->ssao.ssaoText);
	renderData->r_Shader.ssaoPass_Blur_Shader->setInt("ssaoInput", 0);

	world.renderer->DrawQuad(renderData);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

#pragma region Init

std::pair<unsigned int, unsigned int> Systems::RenderSystem::CreateDummyShadowTextures() {
	unsigned int dummyDepthMap2D = 0;
	unsigned int dummyDepthCubeMap = 0;

	glGenTextures(1, &dummyDepthMap2D);
	glBindTexture(GL_TEXTURE_2D, dummyDepthMap2D);

	// Create a 1x1 depth texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 1, 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// CRITICAL: These parameters satisfy the shadow sampler!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// --- 2. Dummy CubeMap Depth Texture (for inactive Point lights) ---
	glGenTextures(1, &dummyDepthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, dummyDepthCubeMap);

	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, 1, 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	return std::make_pair(dummyDepthMap2D, dummyDepthCubeMap);
}

void Systems::RenderSystem::InitMainFrameBuffer(Resource::RenderResource* renderData) {
	///////////////////Init fbo
	glGenFramebuffers(1, &renderData->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->framebuffer);


	//////////////////Init texture depth
	glGenTextures(1, &renderData->finalDepthOutput);
	glBindTexture(GL_TEXTURE_2D, renderData->finalDepthOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderData->finalDepthOutput, 0);


	//////////////////Init texture color [0] and Bloom texture [1] 
	glGenTextures(2, renderData->finalTxtColorOutput);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, renderData->finalTxtColorOutput[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, renderData->finalTxtColorOutput[i], 0);
	}
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);


	//////////////////Assert
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << " The main frame buffer initialisation has failed " << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Systems::RenderSystem::InitQuadVao(Resource::RenderResource* renderData) {
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

void Systems::RenderSystem::InitBloomFBO(Resource::RenderResource* renderData) {
	glGenFramebuffers(2, renderData->pingpongFBO);
	glGenTextures(2, renderData->pingpongBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderData->pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, renderData->pingpongBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0,GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, renderData->pingpongBuffers[i], 0);
	}
}

void Systems::RenderSystem::InitGBuffer(Resource::RenderResource* renderData) {
	glGenFramebuffers(1, &renderData->gBuffer.gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->gBuffer.gBuffer);

	//Position
	glGenTextures(1, &renderData->gBuffer.gPosition);
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderData->gBuffer.gPosition, 0);

	//Normal
	glGenTextures(1, &renderData->gBuffer.gNormal);
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderData->gBuffer.gNormal, 0);

	//Albedo
	glGenTextures(1, &renderData->gBuffer.gAlbedo);
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, renderData->gBuffer.gAlbedo, 0);

	//ARM - Ambient Occlusion Roughness Metallic
	glGenTextures(1, &renderData->gBuffer.gARM);
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gARM);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, renderData->gBuffer.gARM, 0);



	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 , GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	//////////////////Init texture depth
	glGenTextures(1, &renderData->gBuffer.gDepth);
	glBindTexture(GL_TEXTURE_2D, renderData->gBuffer.gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderData->gBuffer.gDepth, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "The intermediate FBO initialisation has failed" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Systems::RenderSystem::InitSSAO(Resource::RenderResource* renderData){
	glGenFramebuffers(1, &renderData->ssao.ssaoBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->ssao.ssaoBuffer);

	glGenTextures(1, &renderData->ssao.ssaoText);
	glBindTexture(GL_TEXTURE_2D, renderData->ssao.ssaoText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderData->ssao.ssaoText, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Generate random point for the sample kernel
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	for (unsigned int i = 0; i < renderData->ssao.kernelSample; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, 
			randomFloats(generator) * 2.0 - 1.0, 
			randomFloats(generator)
		);

		sample = glm::normalize(sample);
		sample *= randomFloats(generator); 
		float scale = (float)i / renderData->ssao.kernelSample;
		scale = glm::mix(0.1f, 1.0f, scale * scale);
		sample *= scale;
		renderData->ssao.ssaoKernel.push_back(sample);

	}
	//Create 4x4 array use to calc TBN in SSAO shader
	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f
		);
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &renderData->ssao.ssao_NoiseText);
	glBindTexture(GL_TEXTURE_2D, renderData->ssao.ssao_NoiseText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT,&ssaoNoise[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Systems::RenderSystem::InitSSAO_Blur(Resource::RenderResource* renderData) {
	glGenFramebuffers(1, &renderData->ssao.ssaoBlur_Buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->ssao.ssaoBlur_Buffer);

	glGenTextures(1, &renderData->ssao.ssaoBlurText);
	glBindTexture(GL_TEXTURE_2D, renderData->ssao.ssaoBlurText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RED, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderData->ssao.ssaoBlurText, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Systems::RenderSystem::InitToImGui_FBO(Resource::RenderResource* renderData) {
	glGenFramebuffers(1, &renderData->toImGui_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->toImGui_FBO);

	glGenTextures(1, &renderData->toImGui_Texture);
	glBindTexture(GL_TEXTURE_2D, renderData->toImGui_Texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->renderWIDTH, renderData->renderHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderData->toImGui_Texture, 0);

	glBindTexture(GL_TEXTURE_2D, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Erreur : FBO incomplet !" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Systems::RenderSystem::Init_AllBuffer(Resource::RenderResource* renderData) {
	InitMainFrameBuffer(renderData);
	InitBloomFBO(renderData);
	InitQuadVao(renderData);
	InitGBuffer(renderData);
	InitSSAO(renderData);
	InitSSAO_Blur(renderData);
	InitToImGui_FBO(renderData);
}

#pragma endregion


// Geometry Pass
void Systems::RenderSystem::RenderScene(World& world, const Resource::ResourceBuffer* resourceBuffer, Component::CameraComponent* mainCamera) {
	glBindFramebuffer(GL_FRAMEBUFFER, resourceBuffer->renderResource->gBuffer.gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	View view = world.view<Component::MeshHandle, Component::Transform, Component::MaterialHandle>();
	view.each([&](int entity, Component::MeshHandle& meshHandle, Component::Transform& transform, Component::MaterialHandle& materialHandle) {
		if (meshHandle.index != -1 && meshHandle.haveToBeDraw) {
			Shader currentShader = world.assetStore->Get_Material(materialHandle.index)->shader;
			Mesh currentMesh = world.assetStore->Get_Mesh(meshHandle.index);


			currentShader.Use();

			// --- Link Matrices ---
			currentShader.setMatrix("view", mainCamera->viewMatrice);
			currentShader.setMatrix("projection", resourceBuffer->renderResource->projection);
			currentShader.setMatrix("model", transform.GetTransformModel());

			world.renderer->DrawMesh(currentMesh);
		}
	});
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Systems::RenderSystem::Init(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::WindowResource* windowData = resourceBuffer->windowResource;
	Resource::RenderResource* renderData = resourceBuffer->renderResource;

	if (windowData->window == nullptr) {
		std::cout << "Reference de la window impossible a recuperer" << std::endl;
		abort();
	}

	Shader::CreateDefaultWhiteTexture();
	Shader::CreateNeutralNormalText();

	std::pair<Material&, int> defaultMat = world.assetStore->CreateMaterial("Default_Material", "TriangleOne/Shader/Geometry_Pass/Vertex_GeometryPass.glsl", "TriangleOne/Shader/Geometry_Pass/Fragment_GeometryPass.glsl");
	defaultMat.first.diffuse_Text_Handle = Shader::GetDefaultText();
	defaultMat.first.normal_Text_Handle = Shader::GetNeutralNormalText();
	renderData->mainMaterialHandle = defaultMat.second;

	std::pair<unsigned  int, unsigned int> shadowDummy = CreateDummyShadowTextures();
	renderData->dummyDepthMap2D = shadowDummy.first;
	renderData->dummyDepthCubeMap = shadowDummy.second;

	renderData->r_Shader.depthShader = std::make_unique<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/DepthMapVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/DepthMapFrag.glsl");
	renderData->r_Shader.depthShaderCubeMap = std::make_unique<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeFrag.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeGeometry.glsl");
	renderData->r_Shader.postProcessShader = std::make_unique<Shader>("TriangleOne/Shader/PostProcessShader/PostProcessVertex.glsl", "TriangleOne/Shader/PostProcessShader/PostProcessFrag.glsl");
	renderData->r_Shader.bloomShader = std::make_unique<Shader>("TriangleOne/Shader/BloomShader/VertexBloom.glsl", "TriangleOne/Shader/BloomShader/FragmentBloom.glsl");
	renderData->r_Shader.lightningPass_Shader = std::make_unique<Shader>("TriangleOne/Shader/Lighting_Pass/Vertex_LightningPass_Shader.glsl", "TriangleOne/Shader/Lighting_Pass/Fragment_LightningPass_Shader.glsl");
	renderData->r_Shader.ssaoPass_Shader = std::make_unique<Shader>("TriangleOne/Shader/SSAO_Pass/Vertex_SSAO_Shader.glsl", "TriangleOne/Shader/SSAO_Pass/Fragment_SSAO_Shader.glsl");
	renderData->r_Shader.ssaoPass_Blur_Shader = std::make_unique<Shader>("TriangleOne/Shader/SSAO_Pass/Vertex_SSAO_Shader.glsl", "TriangleOne/Shader/SSAO_Pass/Fragment_Blur_SSAO_Shader.glsl");
	renderData->r_Shader.equirectangular_To_CubemapShader = std::make_unique<Shader>("TriangleOne/Shader/IBLshader/Vertex_Equirectangular_to_Cubemap.glsl", "TriangleOne/Shader/IBLshader/Fragment_Equirectangular_to_Cubemap.glsl");
	renderData->r_Shader.skyBox_Shader = std::make_unique<Shader>("TriangleOne/Shader/MiscShader/SkyBoxVertex.glsl", "TriangleOne/Shader/MiscShader/SkyBoxFrag.glsl");
	renderData->r_Shader.irradiance_Shader = std::make_unique<Shader>("TriangleOne/Shader/IBLshader/Vertex_Equirectangular_to_Cubemap.glsl", "TriangleOne/Shader/IBLshader/Irradiance/Fragment_Irradiance_Convulation.glsl");
	renderData->r_Shader.prefilter_Shader = std::make_unique<Shader>("TriangleOne/Shader/IBLshader/Vertex_Equirectangular_to_Cubemap.glsl", "TriangleOne/Shader/IBLshader/SpecularIBL/Fragment_Prefilter.glsl");
	renderData->r_Shader.brdf_Shader = std::make_unique<Shader>("TriangleOne/Shader/BRDF/Vertex_brdf.glsl", "TriangleOne/Shader/BRDF/Fragment_brdf.glsl");
	renderData->r_Shader.axisShader = std::make_unique<Shader>("TriangleOne/Shader/MiscShader/AxisShader/AxisVertex.glsl", "TriangleOne/Shader/MiscShader/AxisShader/AxisFrag.glsl");

	//Create the main cam  // TEMP / WARNING
	Entity camEntity = world.Register();
	Component::CameraComponent cameraComponent(windowData->WIDTH, windowData->HEIGHT);
	Component::Transform transform(glm::vec3(0, 5, 10));
	Component::AudioListener audioListener;
	world.add_components(camEntity, cameraComponent, transform, audioListener);

	world.get_ressource<Resource::ActiveCamera>()->cameraID = camEntity;

	Entity model = world.Register();
	Component::Transform modelTransform(glm::vec3(-2,-1,0)); // ("Assets/main_sponza/main_sponza/NewSponza_Main_Yup_003.fbx");
	std::pair<Mesh&, int> value = world.assetStore->Get_Mesh("Assets/ImpScene/autumn_house.glb");
	Component::MeshHandle meshHandle(value.second);
	Component::SceneTag mesh_scene_Tag("Maison");
	Component::MaterialHandle materialHandle(renderData->mainMaterialHandle);
	//Component::ConvexShape meshCollider(modelTransform.position, value.first.Get_VerticesPosition());
	//meshCollider.motionType.Set(JPH::EMotionType::Static);
	Component::BoxCollider boxCollider(modelTransform.position, glm::vec3(100, 2, 100), JPH::EMotionType::Static, false, 1.0f);


	world.add_components(model, mesh_scene_Tag, materialHandle, meshHandle, modelTransform, std::move(boxCollider));

	glEnable(GL_CULL_FACE);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	float intensity = 2.0f;

	glm::vec3 worldLightDir = glm::normalize(glm::vec3(-2.0f, 4.0f, -1.0f));

	float cutOff = 5.5f;
	float outerCutOff = 15.5f;

	Entity dirLight_E = world.Register();
	Component::DirLight dirLight(color, intensity, worldLightDir, renderData->r_Shader.depthShader.get());

	Component::LightToInitTag tag(Component::LightTag::Directional_Tag);
	Component::Transform lightTransform(glm::vec3(0.0f, 4.0f, -6.0f));
	Component::SceneTag dir_scene_Tag("Dir Light");

	world.add_components(dirLight_E, dir_scene_Tag, transform, dirLight, tag);

	/////////////////////////////////////

	Entity spotLightEntity = world.Register();
	Component::Transform spotTransform(glm::vec3(0.0f, 4.0f, -6.0f));
	Component::SpotLight spotLight(color, 10000.0f, glm::vec3(1.0f, 0.0f, 0.0f), cutOff, outerCutOff, 30.0f, renderData->r_Shader.depthShader.get());
	Component::LightToInitTag spotTag(Component::LightTag::SpotLight_Tag);
	Component::SceneTag spotLight_scene_Tag("Spot Light");

	world.add_components(spotLightEntity, spotLight_scene_Tag, spotTransform, spotLight, spotTag);


	/////////////////////////////////////

	Entity pointLightEntity = world.Register();
	Component::Transform transformPointLight(glm::vec3(1.0f, 5.0f, 0.0f));
	Component::PointLight pointLight(color, 800.0f, 8.0f, renderData->r_Shader.depthShaderCubeMap.get());
	Component::LightToInitTag pointTag(Component::LightTag::PointLight_Tag);
	Component::SceneTag pointLight_scene_Tag("Point Light");

	world.add_components(pointLightEntity, pointLight_scene_Tag, transformPointLight, pointLight, pointTag);
	/////////////////////////////////////


	//Entity backpack = world.Register();
	//Component::Transform backPackTransform(glm::vec3(10.0f, 3.0f, 2.0f));
	//backPackTransform.rotation = glm::vec3(-90, 0, 0);
	//std::pair<Mesh&, int> backpackValue = world.assetStore->Get_Mesh("Assets/backpack/backpack.obj");
	//Component::MeshHandle backpackModeleHandle(backpackValue.second);
	//Component::SceneTag backpackTag("backpack");

	//world.add_components(backpack, backPackTransform, backpackTag, materialHandle, backpackModeleHandle);

	//world.renderer->SetViewport_Size(glm::vec2(renderData->renderWIDTH, renderData->renderHEIGHT));

	glEnable(GL_DEPTH_TEST);
	Init_AllBuffer(renderData);
}

void Systems::RenderSystem::Update(World& world, const Resource::ResourceBuffer* resourceBuffer)
 {
	Resource::WindowResource* windowData = resourceBuffer->windowResource;
	Resource::RenderResource* renderData = resourceBuffer->renderResource;
	if (windowData->isIconified) return;

	/////////////////////Camera
	Entity entityCam = resourceBuffer->activeCamera->cameraID;
	Component::CameraComponent* mainCamera = world.get_component<Component::CameraComponent>(entityCam);
	///////////////////
	//Update Projection
	renderData->projection = glm::perspective(glm::radians(mainCamera->zoom), (float)renderData->renderWIDTH / (float)renderData->renderHEIGHT, mainCamera->nearPlane, mainCamera->farPlane);


	RenderScene(world, resourceBuffer, mainCamera);  
	glDisable(GL_CULL_FACE);
	SSAO_Pass(world, renderData, mainCamera);
	glEnable(GL_CULL_FACE);
}

void Systems::RenderSystem::Shutdown(World& world) {
	std::cout << "Shuting down RenderSystem" << std::endl;
}
