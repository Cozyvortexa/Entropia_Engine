#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


class Engine {
public:
	int run();

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);

private:
	int WIDHT = 800;
	int HEIGHT = 600;

};