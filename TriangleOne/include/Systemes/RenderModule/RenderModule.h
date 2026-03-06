#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "ECS/System.h"
#define NOMINMAX

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

	void RenderScene(World& world, const ResourceBuffer* resourceBuffer, WindowResource* windowData);

	void UpdateLight(Shader* shader, std::vector<DirLight*> directionalLightList,
		std::vector <std::pair<PointLight*, Transform*>> pointLightList,
		std::vector<std::pair<SpotLight*, Transform*>> spotLightList);


};