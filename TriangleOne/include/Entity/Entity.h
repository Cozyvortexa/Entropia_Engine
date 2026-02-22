#pragma once
#include "Systemes/RenderModule/Shader.h"

#include "Systemes/RenderModule/ModelClass.h"

#include <utility>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeindex>


//class Entity {
//public:
//	template<typename T, typename... TArgs>
//	T* AddComponent(TArgs&&... args) { // ...and forwards them...
//		// Ensure T inherits from Component
//		static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
//
//		// ...to std::make_unique, which calls the correct constructor (e.g., MaterialOverride(shaderName))
//		components[typeid(T)] = std::make_unique<T>(std::forward<TArgs>(args)...);
//
//		return static_cast<T*>(components[typeid(T)].get());
//	}
//
//	template<typename T>
//	T* GetComponent() {
//		// Ensure T inherits from Component
//		static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
//		auto it = components.find(typeid(T));
//		// We can safely static_cast here because we know T inherits from Component
//		return (it != components.end()) ? static_cast<T*>(it->second.get()) : nullptr;
//	}
//	Entity() {};	
//	template<typename T, typename... TArgs>
//	T* GetOrAddComponent(TArgs&&... args) {
//		static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
//		auto it = components.find(typeid(T));
//		if (it != components.end()) {
//			return static_cast<T*>(it->second.get());
//		}
//		return AddComponent<T>(std::forward<TArgs>(args)...);
//	}
//
//	template<typename T>
//	bool RemoveComponent() {
//		// Ensure T inherits from Component
//		static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
//
//		auto it = components.find(typeid(T));
//		if (it != components.end()) {
//			components.erase(it); // Erase the entry from the map
//			return true;          // Return true indicating successful removal
//		}
//
//		return false; // Return false if the component wasn't found
//	}
//
//	template<typename T>
//	bool HasComponent() {
//		// Ensure T inherits from Component
//		static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
//		return components.count(typeid(T)) > 0;
//	}
//
//	std::string name;
//
//private: 
//	std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
//	int id = 0;  // Go voir apres
//
//	//Pour plus tard
//	//std::vector<std::shared_ptr<Entity>> childrenEntity;
//};
