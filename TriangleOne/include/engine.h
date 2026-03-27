#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>

//#include <Modules/Module.h>
#include <Systems/windowSystem.h>
//#include <Modules/TimeModule/Time.hpp>

#include <iostream>
#include <vector>

#include "ECS/World.h"
#include "ECS/Scheduler.h"
#include "ECS/AssetStore.h"
#include "Render/Renderer.h"

class Engine {
public:
	Engine();
	~Engine();

	int Run();
	void Shutdown();

private:
	void InitEngine();
	int WIDTH = 800;
	int HEIGHT = 600;
	static Engine* instance;
	std::unique_ptr<WindowSystem> window;


	std::unique_ptr<World> world;
	std::unique_ptr<Scheduler> scheduler;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<Renderer> renderer;
};