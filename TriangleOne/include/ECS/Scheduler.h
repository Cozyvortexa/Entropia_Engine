#pragma once
#include "ECS/System.h"
#include "ECS/World.h"
#include "Systems/RenderSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/InputSystem.h"
#include <utility>

#include "Systems/TimeSystem.hpp"
#include <vector>

#include <iostream>
#include "Systems/InterfaceSystem.h"
#include "Systems/AudioSystem.h"
#include "Systems/PhysicsSystem.h"

class Scheduler {
public:
	Scheduler(World* world, Engine::Systems::WindowSystem* window);
	~Scheduler();

	Scheduler* Init();

	void Update();
	void Shutdown();

	void CreateSystemes();
private:
	void FillResourceBuffer();
	std::vector<std::unique_ptr<Engine::Systems::System>> systemes;

	World* world;

	Engine::Systems::WindowSystem* window;  // Systeme a part

	std::unique_ptr<Engine::Resource::ResourceBuffer> resourceBuffer;
};