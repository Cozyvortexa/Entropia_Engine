#include "Systemes/LightSystem.h"

void LightSystem::Update(World& world, const ResourceBuffer* resourceBuffer) {
	InitShadowBuffer(world);



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
			PointLight* currentLight = world.get_component<PointLight>(entity);
			if (currentLight != nullptr) {
				InitCubeMap(currentLight);
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

//void LightSystem::DrawShadowForDirLight(WindowResource* windowData, DirLight* currentLight) {  // Bug sur la window si resize
//	glViewport(0, 0, currentLight->SHADOW_WIDTH, currentLight->SHADOW_HEIGHT);
//	glBindFramebuffer(GL_FRAMEBUFFER, currentLight->depthMapFBO);
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
//	glViewport(0, 0, windowData->WIDHT, windowData->HEIGHT);
//
//}