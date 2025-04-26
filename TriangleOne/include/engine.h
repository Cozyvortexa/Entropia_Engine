#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>

#include <iostream>
#include <vector>


class Engine {
public:
	int Run();

	static void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void ProcessInput(GLFWwindow* window);
	void DrawTriangle();
	void CreateShader(std::string path, int methode);

	void CreateShaderProg();

private:
	int WIDHT = 800;
	int HEIGHT = 600;

	std::vector<unsigned int> shaderListe;
	unsigned int shaderProgram;
};