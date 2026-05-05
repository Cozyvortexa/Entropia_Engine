#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "ECS/System.h"
#include "ECS/Components/Component.h"
#include "ECS/World.h"

#include <iostream>

namespace Engine::Systems {
	class InputSystem : public System {
	public:
		virtual ~InputSystem() = default;
		void Init(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Update(World& world, const Engine::Resource::ResourceBuffer* ressourceBuffer) override;
		void Shutdown(World& world) override;

		void ProcessInput(GLFWwindow* window, Engine::Component::CameraComponent* mainCamera, Engine::Component::Transform* transformMainCamera, float deltaTime, Engine::Resource::InputResource* inputData);
	};
}