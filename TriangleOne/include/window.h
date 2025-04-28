#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "Module.h"
#include "iostream"

class Window : public Module {
public:
	Window();

	static Window* GetInstance();

	static void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void ProcessInput(GLFWwindow* window);

	GLFWwindow* GetWindow() { return window; }

	void Init() override;
	void Update() override;
	void Shutdown() override;

private:
	int WIDHT = 800;
	int HEIGHT = 600;

	static Window* instance;

	GLFWwindow* window = nullptr;
};