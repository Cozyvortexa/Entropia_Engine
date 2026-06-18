#pragma once
#include "Physics/PhysicsHelper.h"
#include "ECS/Components/PhysicComponent.h"

#include "ECS/System.h"
#include "ECS/World.h"
#include "Physics/JoltRenderer.h"


namespace Engine::Systems {
	class PhysicSystem : public Engine::Systems::System {
	public:

		void Init(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Update(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Shutdown(World& world) override;

	private:
		void UpdatePhysicObject(Engine::Component::Transform& transform, Engine::Component::PhysicObject& physicObject);
	};

}
