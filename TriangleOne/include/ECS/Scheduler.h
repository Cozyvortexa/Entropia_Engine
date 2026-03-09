#pragma once
#include "ECS/System.h"
#include "ECS/World.h"
#include "Systems/RenderSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/LightSystem.h"
#include <utility>

#include "Systems/TimeSystem.hpp"
//#include <Systemes/RenderModule/RenderModule.h>
#include <vector>

#include <iostream>

class Scheduler {
public:
	Scheduler(World* world, WindowSystem* window);
	~Scheduler();

	Scheduler* Init();

	void Update();
	void Shutdown();

	void CreateSystemes();
private:
	void FillResourceBuffer();
	std::vector<std::unique_ptr<System>> systemes;

	World* world;

	WindowSystem* window;  // Systeme a part

	std::unique_ptr<ResourceBuffer> resourceBuffer;
};