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

#include <random>

class RenderSystem : public System {
public:
	void gBufferToResolvedBuffer(WindowResource* windowData, RenderResource* renderData);
	void SSAO_Pass(World& world, WindowResource* windowData, RenderResource* renderData);

	void InitQuadVao(WindowResource* windowData, RenderResource* renderData);
	void InitMainFrameBuffer(WindowResource* windowData, RenderResource* renderData);
	void InitIntermediateFBO(WindowResource* windowData, RenderResource* renderData);
	void InitBloomFBO(WindowResource* windowData, RenderResource* renderData);
	void InitGBuffer(WindowResource* windowData, RenderResource* renderData);
	void InitSSAO(WindowResource* windowData, RenderResource* renderData);
	void InitSSAO_Blur(WindowResource* windowData, RenderResource* renderData);
	void InitToImGui_FBO(WindowResource* windowData, RenderResource* renderData);

	void Init_AllBuffer(WindowResource* windowData, RenderResource* renderData);

	std::pair<unsigned int, unsigned int> CreateDummyShadowTextures();

	static void ResizeText(WindowResource* windowData, RenderResource* renderResource);

	void Init(World& world, const ResourceBuffer* resourceBuffer) override;
	void Update(World& world, const ResourceBuffer* ressourceBuffer) override;
	void Shutdown(World& world) override;

	void RenderScene(World& world, const ResourceBuffer* resourceBuffer, WindowResource* windowData, CameraComponent* mainCamera);

};