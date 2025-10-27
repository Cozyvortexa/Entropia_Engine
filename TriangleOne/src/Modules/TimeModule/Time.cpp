#include "Modules/TimeModule/Time.hpp"

Time* Time::instance = nullptr;

float Time::deltaTime = 0.0f;

Time::Time() {
	instance = this;
}

Time* Time::GetInstance() {
	return instance;
}

void Time::Update() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void Time::Shutdown() {
	std::cout << "Shuting down TimeModule" << std::endl;
}
