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


void Systems::PhysicSystem::UpdatePhysicObject(Engine::Component::Transform& transform, Component::PhysicObject& physicObject) {
	JPH::EMotionType motionType = physicObject.motionType.Get();
	if (motionType == JPH::EMotionType::Dynamic) {
		transform.position = Physics::PhysicsHelper::Get_Position(physicObject);
		transform.rotation = Physics::PhysicsHelper::Get_EulerRotation(physicObject);
	}
	else if (motionType == JPH::EMotionType::Kinematic || motionType == JPH::EMotionType::Static) {
		Physics::PhysicsHelper::Set_Position(physicObject, transform.position);
		Physics::PhysicsHelper::Set_Rotation(physicObject, transform.rotation);
	}
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

	const JPH::BodyLockInterfaceLocking& lock_interface = physicData->physics_system.GetBodyLockInterface();


	Engine::Physics::PhysicsHelper::Init(&physicData->physics_system);



	physicData->job_system = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

	//DrawBox
	physicData->joltDebugRenderer = Physics::JoltDebugRenderer_Factory();
	physicData->debugJoltShader = std::make_unique<Shader>("TriangleOne/Shader/Jolt/Vertex_DebugBox.glsl", "TriangleOne/Shader/Jolt/Fragment_DebugBox.glsl");
	//
	//physicData->debug_draw_settings.mDrawShape = true;
	//physicData->debug_draw_settings.mDrawBoundingBox = true;
	//physicData->debug_draw_settings.mDrawVelocity = false;
}

void Systems::PhysicSystem::Update(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) {
	Resource::PhysicsResource* physicData = resourceBuffer->physicsResource;
	Resource::TimeResource* timeData = resourceBuffer->timeResource;

	if (physicData->display_physicsShape) {
		Entity entityCam = world.get_ressource<Resource::ActiveCamera>()->cameraID;
		Component::Transform* mainCamera_transform = world.get_component<Component::Transform>(entityCam);

		physicData->joltDebugRenderer->SetCameraPos(Physics::PhysicsHelper::To_Vec3_JPH(mainCamera_transform->position));
	}


	physicData->timeAccumulator += timeData->deltaTime;
	while (physicData->timeAccumulator >= physicData->physicsDeltaTime) {
		physicData->physics_system.Update(physicData->physicsDeltaTime, 1, physicData->temp_allocator.get(), physicData->job_system.get());
		physicData->timeAccumulator -= physicData->physicsDeltaTime;
	}

	View boxC_View = world.view<Component::Transform, Component::BoxCollider>();
	View sphereC_View = world.view<Component::Transform, Component::SphereCollider>();

	boxC_View.each([&](int entity, Component::Transform& transform, Component::BoxCollider& boxCollider) {
		UpdatePhysicObject(transform, boxCollider);
		if (boxCollider.displayShape) physicData->joltDebugRenderer->AddToPendingDraw(std::make_pair<>(entity, Physics::ShapeType::Box));
	});

	sphereC_View.each([&](int entity, Component::Transform& transform, Component::SphereCollider& sphereCollider) {
		UpdatePhysicObject(transform, sphereCollider);
		if (sphereCollider.displayShape) physicData->joltDebugRenderer->AddToPendingDraw(std::make_pair<>(entity, Physics::ShapeType::Sphere));
	});
}

void Systems::PhysicSystem::Shutdown(World& world) {
	JPH::UnregisterTypes();

	// Destroy the factory
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;

	std::cout << "Shut down PhysicSystem" << std::endl;
}

