#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "ECS/System.h"
#include "ECS/Components/Component.h"
#include "iostream"

#include "ECS/World.h"

class WindowSystem : public System {
public:
	WindowSystem();

	static void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void ProcessInput(GLFWwindow* window);
	bool ShouldClose(World& world);

	void Init(World& world, const ResourceBuffer* resourceBuffer) override;
	void Update(World& world, const ResourceBuffer* resourceBuffer) override;
	void Shutdown(World& world) override;
};