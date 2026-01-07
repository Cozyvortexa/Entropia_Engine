#pragma once
#include "Entity/Components/EntityComponent.h"
#include "Entity/Components/Light.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/Transform.h"
#include "window.h"

#include <vector>

class RenderSystem {
public:
	RenderSystem(unsigned int* newFramebuffer);
	void RenderMesh();

	void AddMeshComponent(std::shared_ptr<MeshComponent> modele);

	void AddLightComponent(std::shared_ptr<DirLight> modele);
	//void RemoveMeshComponent(std::shared_ptr<MeshComponent> modele);

private:
	void UpdateLight(std::shared_ptr<Shader> shader);
	void UpdateShadow();

	void DrawShadowForDirLight(std::shared_ptr<DirLight> currentLight);
	//void DrawShadowPoint(std::shared_ptr<PointLight> currentLight, std::shared_ptr<MeshComponent> currentMesh);
	
	void InitShadowMap();

	glm::mat4 CalculModel(std::shared_ptr<Transform> currentTransform);

	std::vector<std::shared_ptr<MeshComponent>> modeleList;

	std::vector<std::shared_ptr<DirLight>> directionalLightList;
	std::vector<std::shared_ptr<PointLight>> pointLightList;
	std::vector<std::shared_ptr<SpotLight>> spotLightList; 

	glm::mat4 _model = glm::mat4(1.0f);

	unsigned int depthMapFBO = 0;
	unsigned int depthMap = 0;
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	unsigned int* framebuffer = nullptr;
};
