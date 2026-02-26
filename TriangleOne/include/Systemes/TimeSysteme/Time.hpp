#pragma once

#include "ECS/System.h"
#include "Windows.h"
#include <GLFW/glfw3.h>
#include "iostream"

#include "ECS/World.h"

class Time : public System {  // Oui c'est un hpp, time.h existe deja ( utiliser dans une lib )
public:
	void Update(World& world, const ResourceBuffer* ressourceBuffer) override;
	void Shutdown(World& world) override;
};