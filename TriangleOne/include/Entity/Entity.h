#pragma once
#include "Modules/RenderModule/Shader.h"

#include "Modules/RenderModule/ModelClass.h"
#include "Entity/Components/EntityComponent.h"
#include "Entity/Components/Transform.h"

#include <memory>
#include <vector>

class Entity{
public:
	Entity();
	
	template<typename EntityComponent> std::shared_ptr<EntityComponent> EntityCreateModules();


	void EntityRemoveModules(std::shared_ptr<EntityComponent> module);

private: 
	std::shared_ptr<Transform> transform;
	std::vector<std::shared_ptr<EntityComponent>> entityComponentList;



	//Pour plus tard
	//std::vector<std::shared_ptr<Entity>> childrenEntity;
};
