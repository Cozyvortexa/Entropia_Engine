#pragma once
#include "ECS/Components/Component.h"

class World;

namespace Engine::Systems {
	class System {
	public:
		virtual void Init(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) {};
		virtual void Update(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) {};
		virtual void Shutdown(World& world) {};
	};
}