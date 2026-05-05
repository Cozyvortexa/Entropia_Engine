#include "Systems/TimeSystem.hpp"

namespace Resource = Engine::Resource;
namespace Systems = Engine::Systems;

void Systems::TimeSystem::Update(World& world, const Resource::ResourceBuffer* ressourceBuffer) {
	Resource::TimeResource* timeData = ressourceBuffer->timeResource;
	float currentFrame = glfwGetTime();
	timeData->deltaTime = currentFrame - timeData->lastFrame;
	timeData->lastFrame = currentFrame;
}

void Systems::TimeSystem::Shutdown(World& world) {
	std::cout << "Shuting down TimeSysteme" << std::endl;
}
