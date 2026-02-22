#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "ECS/System.h"
#include "iostream"

class Window : public System {
public:
	Window();

	static Window* GetInstance();

	static void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void ProcessInput(GLFWwindow* window);
	bool ShouldClose();

	GLFWwindow* GetWindow() { return window; }

	static int GetWidth(){ return WIDHT; }
	static int GetHeight() { return HEIGHT; }

	void Init() override;
	void Update(World& world) override;
	void Shutdown() override;

private:
	inline static int WIDHT = 800;
	inline static int HEIGHT = 600;

	static Window* instance;

	int sample = 4;

	GLFWwindow* window = nullptr;
};