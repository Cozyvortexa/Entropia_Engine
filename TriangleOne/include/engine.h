#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>

#include <Systems/windowSystem.h>

#include <iostream>
#include <vector>

#include "ECS/World.h"
#include "ECS/Scheduler.h"
#include "ECS/AssetStore.h"
#include "Render/Renderer.h"

class EntropiaZero {
public:
	EntropiaZero();
	~EntropiaZero();

	int Run();
	void Shutdown();

private:
	void InitEngine();
	void RendererFactory();
	int WIDTH = 800;
	int HEIGHT = 600;
	static EntropiaZero* instance;
	std::unique_ptr<Engine::Systems::WindowSystem> window;


	std::unique_ptr<World> world;
	std::unique_ptr<Scheduler> scheduler;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<Renderer> renderer;
};