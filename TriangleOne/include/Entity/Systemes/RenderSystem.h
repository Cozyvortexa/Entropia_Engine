#pragma once
#include "Entity/Entity.h"
#include "Entity/Components/Light.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/Transform.h"
#include "window.h"

#include "Scene.h"

#include <vector>

class RenderSystem {
public:
	RenderSystem(unsigned int* newFramebuffer, Camera* newMainCamera);
	void RenderScene(Scene* scene, glm::mat4 projection);


private:
	void UpdateLight(std::shared_ptr<Shader> shader, std::vector<DirLight*> directionalLightList, std::vector<std::pair<PointLight*, Transform*>> pointLightList);
	void UpdateShadow(Scene* scene, glm::mat4 projection, std::vector<DirLight*> star, std::vector<std::pair<PointLight*, Transform*>> pointLights);

	void DrawShadowForDirLight(DirLight* currentLight, Scene* scene);
	void DrawShadowForPointLight(std::pair<PointLight*, Transform*> currentLight, Scene* scene);
	
	void InitShadowMap();

	glm::mat4 CalculModel(Transform* currentTransform);

	Camera* mainCamera = nullptr;
	glm::mat4 _model = glm::mat4(1.0f);

	unsigned int depthMapFBO = 0;
	unsigned int depthMap = 0;
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	unsigned int* framebuffer = nullptr;
};
