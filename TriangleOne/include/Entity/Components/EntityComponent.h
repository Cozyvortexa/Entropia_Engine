#pragma once

#include <memory>
class Entity;
class Transform;


struct EntityComponent {
public:
	std::shared_ptr<Entity> entity = nullptr;
	std::shared_ptr<Transform> transform = nullptr; 
};