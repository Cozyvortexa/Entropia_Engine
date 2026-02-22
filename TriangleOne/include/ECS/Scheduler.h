#pragma once
#include "ECS/System.h"
#include "ECS/World.h"
#include "Systemes/TimeSysteme/Time.hpp"
//#include <Systemes/RenderModule/RenderModule.h>
#include <vector>

#include <iostream>

class Scheduler {
public:
	Scheduler(World* world, Window* window);
	~Scheduler();

	Scheduler* Init();

	void Update();
	void Shutdown();

	void CreateSystemes();
private:
	std::vector<std::unique_ptr<System>> systemes;

	World* world;
	Window* window;  // Systeme a part
};