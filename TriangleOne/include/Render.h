#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "Module.h"

#include <fstream>
#include <sstream>

#include <iostream>
#include <vector>



class Render : public Module {
public:

	static void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void ProcessInput(GLFWwindow* window);
	void DrawTriangle();
	void CreateShader(std::string path, int methode);

	void CreateShaderProg();


	void Update() override;
	void Init() override;
	void Shutdown() override;

private:
	int WIDHT = 800;
	int HEIGHT = 600;

	GLFWwindow* window = nullptr;

	std::vector<unsigned int> shaderListe;
	unsigned int shaderProgram;
};