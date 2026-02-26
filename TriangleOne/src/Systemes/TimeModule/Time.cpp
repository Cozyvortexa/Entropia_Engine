#include "Systemes/TimeSysteme/Time.hpp"


void Time::Update(World& world, const ResourceBuffer* ressourceBuffer) {
	TimeResource* timeData = ressourceBuffer->timeResource;
	float currentFrame = glfwGetTime();
	timeData->deltaTime = currentFrame - timeData->lastFrame;
	timeData->lastFrame = currentFrame;
}

void Time::Shutdown(World& world) {
	std::cout << "Shuting down TimeSysteme" << std::endl;
}
