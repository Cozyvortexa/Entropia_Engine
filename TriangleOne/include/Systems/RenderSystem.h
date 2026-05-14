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
namespace Engine::Systems {
	class RenderSystem : public System {
	public:
		void SSAO_Pass(World& world, Engine::Resource::RenderResource* renderData, Engine::Component::CameraComponent* mainCamera);

		void InitQuadVao(Engine::Resource::RenderResource* renderData);
		void InitMainFrameBuffer(Engine::Resource::RenderResource* renderData);
		void InitBloomFBO(Engine::Resource::RenderResource* renderData);
		void InitGBuffer(Engine::Resource::RenderResource* renderData);
		void InitSSAO(Engine::Resource::RenderResource* renderData);
		void InitSSAO_Blur(Engine::Resource::RenderResource* renderData);
		void InitToImGui_FBO(Engine::Resource::RenderResource* renderData);

		void Init_AllBuffer(Engine::Resource::RenderResource* renderData);

		std::pair<unsigned int, unsigned int> CreateDummyShadowTextures();

		static void ResizeFrameBufferText(Engine::Resource::RenderResource* renderResource);

		void Init(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Update(World& world, const Engine::Resource::ResourceBuffer* ressourceBuffer) override;
		void Shutdown(World& world) override;

		void RenderScene(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer, Engine::Component::CameraComponent* mainCamera);

	};
}