#pragma once

#include "ECS/System.h"
#include "Windows.h"
#include <GLFW/glfw3.h>
#include "iostream"

#include "ECS/World.h"

namespace Engine::Systems {
	class TimeSystem : public System {
	public:
		void Update(World& world, const Engine::Resource::ResourceBuffer* ressourceBuffer) override;
		void Shutdown(World& world) override;
	};
}