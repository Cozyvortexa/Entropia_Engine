#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


class Engine {
public:
	int Run();

	static void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void ProcessInput(GLFWwindow* window);
	void DrawTriangle();
	void CreateShader();

private:
	int WIDHT = 800;
	int HEIGHT = 600;


};