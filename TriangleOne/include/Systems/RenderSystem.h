#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "ECS/System.h"
#define NOMINMAX

#include <iostream>
#include <vector>

#include "Render/Shader.h"
#include "Systems/windowSystem.h"
#include "Systems/TimeSystem.hpp"
#include "Render/Texture.h"
#include "Systems/CameraSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Render/Mesh.h"
#include "Render/Renderer.h"

#include "ECS/World.h"
#include "ECS/Components/Light.h"

class RenderSystem : public System {
public:
	void DrawTextureOnScreen(WindowResource* windowData, RenderResource* renderData);

	void InitQuadVao(WindowResource* windowData, RenderResource* renderData);
	void InitMainFrameBuffer(WindowResource* windowData, RenderResource* renderData);


	//void InitSkyBox();
	//void DrawSkyBox(glm::mat4 projectionMatrix);

	void Init(World& world, const ResourceBuffer* resourceBuffer) override;
	void Update(World& world, const ResourceBuffer* ressourceBuffer) override;
	void Shutdown(World& world) override;

	void RenderScene(World& world, const ResourceBuffer* resourceBuffer, WindowResource* windowData);

};