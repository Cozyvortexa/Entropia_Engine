#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "ECS/System.h"


#include <iostream>
#include <vector>

#include "Shader.h"
#include "window.h"
#include "Systemes/TimeSysteme/Time.hpp"
#include "Texture.h"
#include "CameraSys.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ModelClass.h"


#include "Scene.h"
#include "Entity/Entity.h"
#include "Entity/Components/Light.h"

#include "ECS/World.h"

class RenderModule : public System {
public:
	void DrawTextureOnScreen(WindowResource* windowData, RenderResource* renderData);

	void InitQuadVao(WindowResource* windowData, RenderResource* renderData);


	//void InitSkyBox();
	//void DrawSkyBox(glm::mat4 projectionMatrix);

	void Init(World& world) override;
	void Update(World& world, const ResourceBuffer* ressourceBuffer) override;
	void Shutdown(World& world) override;

	glm::mat4 CalculModel(Transform* currentTransform, glm::mat4 _model);

	void RenderScene(WindowResource* windowData, RenderResource* renderData, glm::mat4 projection);

	void UpdateLight(std::shared_ptr<Shader> shader, std::vector<DirLight*> directionalLightList,
		std::vector <std::pair<PointLight*, Transform*>> pointLightList,
		std::vector<std::pair<SpotLight*, Transform*>> spotLightList);


	void DrawShadowForDirLight(WindowResource* windowData, RenderResource* renderData, DirLight* currentLight);
	void DrawShadowForPointLight(std::pair<PointLight*, Transform*> currentLight);
	void DrawShadowForSpotLight(std::pair<SpotLight*, Transform*> currentLight);

	void UpdateShadow(WindowResource* windowData, RenderResource* renderData, glm::mat4 projection, std::vector <DirLight*> star,
		std::vector<std::pair<PointLight*, Transform*>> pointLights,
		std::vector<std::pair<SpotLight*, Transform*>> spotLights);
};