#include "Systems/TimeSystem.hpp"


void TimeSystem::Update(World& world, const ResourceBuffer* ressourceBuffer) {
	TimeResource* timeData = ressourceBuffer->timeResource;
	float currentFrame = glfwGetTime();
	timeData->deltaTime = currentFrame - timeData->lastFrame;
	timeData->lastFrame = currentFrame;
}

void TimeSystem::Shutdown(World& world) {
	std::cout << "Shuting down TimeSysteme" << std::endl;
}
