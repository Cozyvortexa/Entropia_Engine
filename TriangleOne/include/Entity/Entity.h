#pragma once
#include "Modules/RenderModule/Shader.h"

#include "Modules/RenderModule/ModelClass.h"
#include "Entity/Components/EntityComponent.h"
#include "Entity/Components/Transform.h"

#include <utility>
#include <memory>
#include <string>
#include <vector>

class Entity{
public:
	Entity();
	
	template<typename T, typename... Args> std::shared_ptr<T> EntityCreateModules(Args&&... args) {
		auto module = std::make_shared<T>(std::forward<Args>(args)...);
		entityComponentList.push_back(module);
		return module;
	}

	void EntityRemoveModules(std::shared_ptr<EntityComponent> module);

	std::string name;
private: 
	std::shared_ptr<Transform> transform;
	std::vector<std::shared_ptr<EntityComponent>> entityComponentList;
	int id;

	//Pour plus tard
	//std::vector<std::shared_ptr<Entity>> childrenEntity;
};
