#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "Module.h"

#include <fstream>
#include <sstream>

#include <iostream>
#include <vector>

#include "Shader.h"
#include "window.h"



class RenderModule : public Module {
public:
	RenderModule();

	static RenderModule* GetInstance();

	void DrawTriangle();
	void DrawRectangle();
	void Texture();

	void Init() override;
	void Render() override;
	void Shutdown() override;

private:
	Shader* shader = nullptr;
	GLFWwindow* window = nullptr;

	static RenderModule* instance;

	std::vector<unsigned int> shaderListe;
};