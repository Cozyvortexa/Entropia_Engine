#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>

#include <Module.h>
#include <RenderModule.h>
#include <window.h>
#include <Time.hpp>

#include <iostream>
#include <vector>


class Engine {
public:
	Engine();
	~Engine();

	int Run();

	Engine* Init();
	void CreateModules();
	void Update();
	void Render();
	void Shutdown();


private:
	int WIDHT = 800;
	int HEIGHT = 600;
	static Engine* instance;
	Window* window = nullptr;

	std::vector<Module*> modules;
	std::vector<unsigned int> shaderListe;
	unsigned int shaderProgram;

	template<typename ModuleType> ModuleType* CreateModule();
};