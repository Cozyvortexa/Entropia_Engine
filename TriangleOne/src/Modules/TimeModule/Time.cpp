#include "Systemes/TimeSysteme/Time.hpp"


void Time::Update(World& world) {
	TimeRessource* ressource = world.get_ressource<TimeRessource>();
	float currentFrame = glfwGetTime();
	ressource->deltaTime = currentFrame - ressource->lastFrame;
	ressource->lastFrame = currentFrame;
}

void Time::Shutdown(World& world) {
	std::cout << "Shuting down TimeSysteme" << std::endl;
}
