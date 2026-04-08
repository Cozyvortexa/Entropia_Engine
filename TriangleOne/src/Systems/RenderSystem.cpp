#include <Systems/RenderSystem.h>

// To resolved the MSAA, write in the resolved 2D texture
void RenderSystem::gBufferToResolvedBuffer(WindowResource* windowData, RenderResource* renderData) {

	glBindFramebuffer(GL_READ_FRAMEBUFFER, renderData->gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderData->intermediateFBO);

	//Position Text
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, windowData->WIDTH, windowData->HEIGHT, 0, 0, windowData->WIDTH, windowData->HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	//Normal Text
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, windowData->WIDTH, windowData->HEIGHT, 0, 0, windowData->WIDTH, windowData->HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	//Albedo Text
	glReadBuffer(GL_COLOR_ATTACHMENT2);
	glDrawBuffer(GL_COLOR_ATTACHMENT2);
	glBlitFramebuffer(0, 0, windowData->WIDTH, windowData->HEIGHT, 0, 0, windowData->WIDTH, windowData->HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBlitFramebuffer(0, 0, windowData->WIDTH, windowData->HEIGHT, 0, 0, windowData->WIDTH, windowData->HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void RenderSystem::SSAO_Pass(World& world, WindowResource* windowData, RenderResource* renderData) {
	///////////////SSAO
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->ssaoBuffer);
	renderData->ssaoPass_Shader->Use();
	renderData->ssaoPass_Shader->setMatrix("projection", renderData->projection);
	renderData->ssaoPass_Shader->setMatrix("invProjection", glm::inverse(renderData->projection));
	renderData->ssaoPass_Shader->setVec("samples", renderData->ssaoKernel);  
	renderData->ssaoPass_Shader->setInt("kernelNbr", renderData->kernelSample);

	renderData->ssaoPass_Shader->setVec("noiseScale", glm::vec2(windowData->HEIGHT / 4.0f, windowData->WIDTH / 4.0f));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderData->gNormalResolved);
	renderData->ssaoPass_Shader->setInt("gNormal", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, renderData->ssao_NoiseText);
	renderData->ssaoPass_Shader->setInt("texNoise", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, renderData->gDepthResolved);
	renderData->ssaoPass_Shader->setInt("gDepthMap", 2);

	world.renderer->DrawQuad(renderData);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	///////////////Blur
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->ssaoBlur_Buffer);
	renderData->ssaoPass_Blur_Shader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderData->ssaoText);
	renderData->ssaoPass_Blur_Shader->setInt("ssaoInput", 0);

	world.renderer->DrawQuad(renderData);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

inline float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}


#pragma region Init

std::pair<unsigned int, unsigned int> RenderSystem::CreateDummyShadowTextures() {
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
	glGenFramebuffers(1, &dummyDepthCubeMap);
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

void RenderSystem::InitMainFrameBuffer(WindowResource* windowData, RenderResource* renderData) {
	///////////////////Init fbo
	glGenFramebuffers(1, &renderData->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->framebuffer);


	//////////////////Init texture depth
	glGenTextures(1, &renderData->finalTxtOutput);
	glBindTexture(GL_TEXTURE_2D, renderData->finalTxtOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windowData->WIDTH, windowData->HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderData->finalTxtOutput, 0);


	//////////////////Init texture color [0] and Bloom texture [1] 
	glGenTextures(2, renderData->finalTxtColorOutput);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, renderData->finalTxtColorOutput[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

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

	//Position
	glGenTextures(1, &renderData->gPositionResolved);
	glBindTexture(GL_TEXTURE_2D, renderData->gPositionResolved);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderData->gPositionResolved, 0);

	//Normal
	glGenTextures(1, &renderData->gNormalResolved);
	glBindTexture(GL_TEXTURE_2D, renderData->gNormalResolved);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderData->gNormalResolved, 0);

	//Albedo
	glGenTextures(1, &renderData->gAlbedoResolved);
	glBindTexture(GL_TEXTURE_2D, renderData->gAlbedoResolved);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, renderData->gAlbedoResolved, 0);



	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 , GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	//////////////////Init texture depth
	glGenTextures(1, &renderData->gDepthResolved);
	glBindTexture(GL_TEXTURE_2D, renderData->gDepthResolved);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windowData->WIDTH, windowData->HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderData->gDepthResolved, 0);

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

void RenderSystem::InitGBuffer(WindowResource* windowData, RenderResource* renderData) {
	glGenFramebuffers(1, &renderData->gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->gBuffer);

	glGenTextures(1, &renderData->gPosition);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->gPosition);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderData->sample, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, renderData->gPosition, 0);


	glGenTextures(1, &renderData->gNormal);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->gNormal);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderData->sample, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, renderData->gNormal, 0);


	glGenTextures(1, &renderData->gAlbedo);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->gAlbedo);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderData->sample, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D_MULTISAMPLE, renderData->gAlbedo, 0);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);


	glGenRenderbuffers(1, &renderData->gDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, renderData->gDepth);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, renderData->sample, GL_DEPTH_COMPONENT24, windowData->WIDTH, windowData->HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderData->gDepth);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "gBuffer initialisation has failed" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::InitSSAO(WindowResource* windowData, RenderResource* renderData){
	glGenFramebuffers(1, &renderData->ssaoBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->ssaoBuffer);

	glGenTextures(1, &renderData->ssaoText);
	glBindTexture(GL_TEXTURE_2D, renderData->ssaoText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowData->WIDTH, windowData->HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderData->ssaoText, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Generate random point for the sample kernel
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	for (unsigned int i = 0; i < renderData->kernelSample; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, 
			randomFloats(generator) * 2.0 - 1.0, 
			randomFloats(generator)
		);

		sample = glm::normalize(sample);
		sample *= randomFloats(generator); float scale = (float)i / renderData->kernelSample;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		renderData->ssaoKernel.push_back(sample);

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

	glGenTextures(1, &renderData->ssao_NoiseText);
	glBindTexture(GL_TEXTURE_2D, renderData->ssao_NoiseText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT,&ssaoNoise[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderSystem::InitSSAO_Blur(WindowResource* windowData, RenderResource* renderData) {
	glGenFramebuffers(1, &renderData->ssaoBlur_Buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->ssaoBlur_Buffer);

	glGenTextures(1, &renderData->ssaoBlurText);
	glBindTexture(GL_TEXTURE_2D, renderData->ssaoBlurText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowData->WIDTH, windowData->HEIGHT, 0, GL_RED, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderData->ssaoBlurText, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::InitToImGui_FBO(WindowResource* windowData, RenderResource* renderData) {
	glGenFramebuffers(1, &renderData->toImGui_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, renderData->toImGui_FBO);

	glGenTextures(1, &renderData->toImGui_Texture);
	glBindTexture(GL_TEXTURE_2D, renderData->toImGui_Texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

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

void RenderSystem::Init_AllBuffer(WindowResource* windowData, RenderResource* renderData) {
	InitMainFrameBuffer(windowData, renderData);
	InitIntermediateFBO(windowData, renderData);
	InitBloomFBO(windowData, renderData);
	InitQuadVao(windowData, renderData);
	InitGBuffer(windowData, renderData);
	InitSSAO(windowData, renderData);
	InitSSAO_Blur(windowData, renderData);
	//InitToImGui_FBO(windowData, renderData);
}

#pragma endregion

//Call when the viewport is re-scall
void RenderSystem::ResizeText(WindowResource* windowData, RenderResource* renderData) {
	glBindTexture(GL_TEXTURE_2D, renderData->finalTxtOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windowData->WIDTH, windowData->HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, renderData->finalTxtColorOutput[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, renderData->finalTxtColorOutput[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

	//Intermediate FBO - Gbuffer
	glBindTexture(GL_TEXTURE_2D, renderData->gPositionResolved);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, renderData->gNormalResolved);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, renderData->gAlbedoResolved);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, renderData->gDepthResolved);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windowData->WIDTH, windowData->HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	//Bloom
	glBindTexture(GL_TEXTURE_2D, renderData->pingpongBuffers[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, renderData->pingpongBuffers[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
	//Gbuffer
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->gPosition);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderData->sample, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->gNormal);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderData->sample, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderData->gAlbedo);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderData->sample, GL_RGBA16F, windowData->WIDTH, windowData->HEIGHT, GL_TRUE);
	glBindRenderbuffer(GL_RENDERBUFFER, renderData->gDepth);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, renderData->sample, GL_DEPTH_COMPONENT24, windowData->WIDTH, windowData->HEIGHT);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	//SSAO
	glBindTexture(GL_TEXTURE_2D, renderData->ssaoText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowData->WIDTH, windowData->HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	//SSAO-Blur
	glBindTexture(GL_TEXTURE_2D, renderData->ssaoBlurText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowData->WIDTH, windowData->HEIGHT, 0, GL_RED, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// Geometry Pass
void RenderSystem::RenderScene(World& world, const ResourceBuffer* resourceBuffer, WindowResource* windowData, CameraComponent* mainCamera) {
	glBindFramebuffer(GL_FRAMEBUFFER, resourceBuffer->renderResource->gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	View view = world.view<MeshHandle, Transform, MaterialHandle>();
	view.each([&](int entity, MeshHandle& meshHandle, Transform& transform, MaterialHandle& materialHandle) {
		if (meshHandle.haveToBeDraw) {
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

void RenderSystem::Init(World& world, const ResourceBuffer* resourceBuffer) {
	WindowResource* windowData = world.get_ressource<WindowResource>();
	RenderResource* renderData = world.get_ressource<RenderResource>();

	if (windowData->window == nullptr) {
		std::cout << "Reference de la window impossible a recuperer" << std::endl;
		abort();
	}

	Shader::CreateDefaultWhiteTexture();
	Shader::CreateNeutralNormalText();

	std::pair<Material&, int> defaultMat = world.assetStore->CreateMaterial("Default_Material", "TriangleOne/Shader/MainShader/BaseVertexShader.glsl", "TriangleOne/Shader/MainShader/BaseFragmentShader.glsl");
	defaultMat.first.diffuse_Text_Handle = Shader::GetDefaultText();
	defaultMat.first.normal_Text_Handle = Shader::GetNeutralNormalText();
	defaultMat.first.specular_Text_Handle = -1;
	renderData->mainMaterialHandle = defaultMat.second;

	std::pair<unsigned  int, unsigned int> shadowDummy = CreateDummyShadowTextures();
	renderData->dummyDepthMap2D = shadowDummy.first;
	renderData->dummyDepthCubeMap = shadowDummy.second;

	renderData->depthShader = std::make_unique<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/DepthMapVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/DepthMapFrag.glsl");
	renderData->depthShaderCubeMap = std::make_unique<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeFrag.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeGeometry.glsl");
	renderData->postProcessShader = std::make_unique<Shader>("TriangleOne/Shader/PostProcessShader/PostProcessVertex.glsl", "TriangleOne/Shader/PostProcessShader/PostProcessFrag.glsl");
	renderData->bloomShader = std::make_unique<Shader>("TriangleOne/Shader/BloomShader/VertexBloom.glsl", "TriangleOne/Shader/BloomShader/FragmentBloom.glsl");
	renderData->lightningPass_Shader = std::make_unique<Shader>("TriangleOne/Shader/Lighting_Pass/Vertex_LightningPass_Shader.glsl", "TriangleOne/Shader/Lighting_Pass/Fragment_LightningPass_Shader.glsl");
	renderData->ssaoPass_Shader = std::make_unique<Shader>("TriangleOne/Shader/SSAO_Pass/Vertex_SSAO_Shader.glsl", "TriangleOne/Shader/SSAO_Pass/Fragment_SSAO_Shader.glsl");
	renderData->ssaoPass_Blur_Shader = std::make_unique<Shader>("TriangleOne/Shader/SSAO_Pass/Vertex_SSAO_Shader.glsl", "TriangleOne/Shader/SSAO_Pass/Fragment_Blur_SSAO_Shader.glsl");

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
	PointLight pointLight(ambient, diffuse, specular, 8.0f, renderData->depthShaderCubeMap.get(), 10.0f);
	LightToInitTag pointTag(LightTag::PointLight_Tag);

	world.add_components(pointLightEntity, transformPointLight, pointLight, pointTag);

	/////////////////////////////////////

	//Entity backpack = world.Register();
	//Transform backPackTransform(glm::vec3(10.0f, 3.0f, 2.0f));
	//backPackTransform.rotation = glm::vec3(-90, 0, 0);
	//std::pair<Mesh&, int> backpackValue = world.assetStore->Get_Mesh("Assets/backpack/backpack.obj");
	//MeshHandle backpackModeleHandle(backpackValue.second);

	//world.add_components(backpack, backPackTransform, sceneTag, materialHandle, backpackModeleHandle);


	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);


	Init_AllBuffer(windowData, renderData);
}

void RenderSystem::Update(World& world, const ResourceBuffer* resourceBuffer)
 {
	WindowResource* windowData = resourceBuffer->windowResource;
	RenderResource* renderData = resourceBuffer->renderResource;

	/////////////////////Camera
	Entity entityCam = resourceBuffer->activeCamera->cameraID;
	CameraComponent* mainCamera = world.get_component<CameraComponent>(entityCam);
	///////////////////
	//Update Projection
	renderData->projection = glm::perspective(glm::radians(mainCamera->zoom), (float)windowData->WIDTH / (float)windowData->HEIGHT, mainCamera->nearPlane, mainCamera->farPlane);


	RenderScene(world, resourceBuffer, windowData, mainCamera);  
	gBufferToResolvedBuffer(windowData, renderData);
	SSAO_Pass(world, windowData, renderData);
}

void RenderSystem::Shutdown(World& world) {
	std::cout << "Shuting down RenderSystem" << std::endl;
}
