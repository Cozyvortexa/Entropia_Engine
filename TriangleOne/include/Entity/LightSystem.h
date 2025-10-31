#pragma once
#include "Entity/EntityComponent.h"

#include <vector>

class LightSystem {
public:
	void UpdateLight();

private:
	std::vector<EntityComponent> directionalLight;
	std::vector<EntityComponent> pointLight;
	std::vector<EntityComponent> spotLight;
};
