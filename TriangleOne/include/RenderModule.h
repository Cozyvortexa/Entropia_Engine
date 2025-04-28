#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "Module.h"

#include <fstream>
#include <sstream>

#include <iostream>
#include <vector>

#include "RenderModule.h"
#include "window.h"



class RenderModule : public Module {
public:
	RenderModule();

	static RenderModule* GetInstance();


	void DrawTriangle();
	void CreateShader(std::string path, int methode);

	void CreateShaderProg();


	void Init() override;
	void Update() override;
	void Shutdown() override;

private:

	GLFWwindow* window = nullptr;

	static RenderModule* instance;

	std::vector<unsigned int> shaderListe;
	unsigned int shaderProgram = NULL;
};