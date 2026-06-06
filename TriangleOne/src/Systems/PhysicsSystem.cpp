#include "Systems/PhysicsSystem.h"

namespace Systems = Engine::Systems;
namespace Resource = Engine::Resource;
namespace Component = Engine::Component;
namespace Physics = Engine::Physics;

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint32_t inLine)
{
	// Print to the TTY
	std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << std::endl;

	// Breakpoint
	return true;
};

#endif // JPH_ENABLE_ASSERTS

static void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	std::cout << buffer << std::endl;
}


void Systems::PhysicSystem::Init(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) {
	Resource::PhysicsResource* physicData = resourceBuffer->physicsResource;
	JPH::RegisterDefaultAllocator();

	JPH::Trace = TraceImpl;
	#ifndef NDEBUG
	JPH::JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl);
	#endif

	JPH::Factory::sInstance = new JPH::Factory();

	JPH::RegisterTypes();

	physicData->temp_allocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

	JPH::JobSystemThreadPool job_system(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);

	physicData->physics_system.Init(physicData->cMaxBodies, physicData->cNumBodyMutexes, physicData->cMaxBodyPairs, physicData->cMaxContactConstraints, physicData->broad_phase_layer_interface, 
		physicData->object_vs_broadphase_layer_filter, physicData->object_vs_object_layer_filter);


	// A body activation listener gets notified when bodies activate and go to sleep
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	physicData->physics_system.SetBodyActivationListener(&physicData->body_activation_listener);

	// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	physicData->contact_listener.world = &world;
	physicData->physics_system.SetContactListener(&physicData->contact_listener);


	Engine::Physics::PhysicsHelper::Init(physicData->physics_system.GetBodyInterface());



	physicData->job_system = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);


}

void Systems::PhysicSystem::Update(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) {
	Resource::PhysicsResource* physicData = resourceBuffer->physicsResource;
	Resource::TimeResource* timeData = resourceBuffer->timeResource;
	const float cDeltaTime = 1.0f / 60.0f;
	const int cCollisionSteps = 1;

	physicData->physics_system.Update(timeData->deltaTime, cCollisionSteps, physicData->temp_allocator.get(), physicData->job_system.get());

	View view = world.view<Component::Transform, Component::BoxCollider>();

	view.each([&](int entity, Component::Transform& transform, Component::BoxCollider& boxCollider) {
		JPH::BodyID currentBody = boxCollider.GetBodyID();
		if (boxCollider.motionType.Get() != JPH::EMotionType::Static) {
			transform.position = Physics::PhysicsHelper::GetCenterOfMass(currentBody);
			transform.rotation = Physics::PhysicsHelper::GetRotation(currentBody);
		}

		});

}

void Systems::PhysicSystem::Shutdown(World& world) {
	JPH::UnregisterTypes();

	// Destroy the factory
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;

	std::cout << "Shut down PhysicSystem" << std::endl;
}

