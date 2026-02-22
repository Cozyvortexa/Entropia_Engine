#pragma once

#include "ECS/System.h"
#include "Windows.h"
#include <GLFW/glfw3.h>
#include "iostream"


class Time : public System {  // Oui c'est un hpp, time.h existe deja ( utiliser dans une lib )
public:
	Time();

	static Time* GetInstance();

	void Update(World& world) override;
	void Shutdown() override;
	static float GetDeltaTime() { return deltaTime; }

private:
	static float deltaTime;
	float lastFrame = 0.0f;

	static Time* instance;
};