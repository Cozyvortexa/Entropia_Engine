#pragma once
#include <ECS/Components/PhysicsStruct.h>
#include "Physics/PhysicsHelper.h"

#include "ECS/System.h"
#include "ECS/World.h"


namespace Engine::Systems {
	class PhysicSystem : public Engine::Systems::System {

		void Init(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Update(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Shutdown(World& world) override;
	};

}
