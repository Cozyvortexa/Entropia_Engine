#pragma once

#include "Module.h"
#include "Windows.h"
#include <GLFW/glfw3.h>


class Time : public Module {  // Oui c'est un hpp, time.h existe deja ( utiliser dans une lib )
public:
	Time();

	static Time* GetInstance();

	void Update() override;
	static float GetDeltaTime() { return deltaTime; }

private:
	static float deltaTime;
	float lastFrame = 0.0f;

	static Time* instance;
};