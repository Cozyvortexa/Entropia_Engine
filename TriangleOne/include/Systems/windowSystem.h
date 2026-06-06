#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "ECS/System.h"
#include "iostream"

#include "ECS/World.h"

namespace Engine::Systems {
	class WindowSystem : public System {
	public:
		WindowSystem();

		static void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
		void ProcessInput(GLFWwindow* window);
		bool ShouldClose(World& world);
		void Window_iconify_callback(GLFWwindow* window, int iconified);

		void Init(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Update(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Shutdown(World& world) override;
	};
}