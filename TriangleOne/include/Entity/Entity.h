#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Modules/RenderModule/Shader.h"

#include "Modules/RenderModule/ModelClass.h"
#include "Entity/EntityComponent.h"

#include <memory>
#include <vector>

class Entity{
public:
	Entity(std::string model);

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(1.0f);
	glm::vec3 scale = glm::vec3(1.0f);

private: 
	std::vector<std::shared_ptr<EntityComponent>> entityComponentList;
	template<typename EntityComponent> std::shared_ptr<EntityComponent> EntityCreateModules();
	void EntityRemoveModules(std::shared_ptr<EntityComponent> module);

	//Pour plus tard
	//std::vector<std::shared_ptr<Entity>> childrenEntity;
};
