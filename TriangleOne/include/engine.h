#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>

//#include <Modules/Module.h>
#include <window.h>
//#include <Modules/TimeModule/Time.hpp>

#include <iostream>
#include <vector>

#include "ECS/World.h"
#include "ECS/Scheduler.h"
#include "ECS/ModelStore.h"

class Engine {
public:
	Engine();
	~Engine();

	int Run();
	void Shutdown();

private:
	void InitEngine();
	int WIDHT = 800;
	int HEIGHT = 600;
	static Engine* instance;
	std::unique_ptr<Window> window;


	std::unique_ptr<World> world;
	std::unique_ptr<Scheduler> scheduler;
	std::unique_ptr<ModelStore> modelStore;
};