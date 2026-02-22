#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "ECS/System.h"
#include "ECS/Component.h"
#include "iostream"

#include "ECS/World.h"

class Window : public System {
public:
	Window();

	static Window* GetInstance();

	static void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void ProcessInput(GLFWwindow* window);
	bool ShouldClose(World& world);

	void Init(World& world) override;
	void Update(World& world) override;
	void Shutdown(World& world) override;
};