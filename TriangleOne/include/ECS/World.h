#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <Systems/windowSystem.h>
#include <tuple>

#include <iostream>
#include <vector>
#include <deque>
#include "ECS/SpareSet.h"
#include "ECS/Components/Component.h"
#include "ECS/AssetStore.h"

#include "Render/Renderer.h"
#include "Audio/AudioHelper.h"

#include <unordered_map>
#include <typeindex>
#include <cassert>
#include <algorithm>
#include <memory>
#include <type_traits>


template<typename... Components>
class View {
private:
    std::tuple<SparseSet<Components>*...> pools;

public:
    View(SparseSet<Components>*... ptrs) : pools(ptrs...) {}

    template<typename Func>
    void each(Func callback) {
        size_t min_size = SIZE_MAX;
        std::vector<Entity>* lead_entities = nullptr;

        //Determine the lead_pool
        ([&](auto* pool) {
            if (pool->dense_entities.size() < min_size) {
                min_size = pool->dense_entities.size();
                lead_entities = &pool->dense_entities;
            }
        }(std::get<SparseSet<Components>*>(pools)), ...);

        //Call the callback func if the entity have all the components 
        for (Entity entity : *lead_entities) {
            if ((std::get<SparseSet<Components>*>(pools)->has(entity) && ...)) {
                callback(entity, std::get<SparseSet<Components>*>(pools)->get(entity)...);
            }
        }
    }
};

class World {
public:
    World(AssetStore* assetStore) {
        this->assetStore = assetStore;
    }

    Entity Register() {
        Entity newEntity;
        if (entity_emptySlot.size() != 0 ) {
            newEntity = entity_emptySlot.back();
            entity_emptySlot.pop_back();
            return newEntity;
        }
        newEntity = entity_Register++;
        return newEntity;
    }
    void Delete_Entity(Entity entity) {
        for (auto& currrentPool : pools) {
            currrentPool.second.get()->Remove(entity);
        }
        entity_emptySlot.push_back(entity);
    }

    template<typename... Components>
    View<Components...> view() {
        return View<Components...>(get_pool<Components>()...);
    }

    template<typename T>
    void add_component(int entity, T&& component) {
        static_assert(std::is_base_of_v<Engine::Component::Component, std::decay_t<T>>, "T must inherit from Engine::Component::Component");

        get_pool<std::decay_t<T>>()->insert(entity, std::forward<T>(component));
    }
    template<typename... Args>
    void add_components(int entity, Args&&... args) {
        static_assert((std::is_base_of_v<Engine::Component::Component, std::decay_t<Args>> && ...), "T must inherit from Engine::Component::Component");

        (add_component(entity, std::forward<Args>(args)), ...);
    }
    template<typename T>
    T* get_component(Entity entity) {
        static_assert(std::is_base_of<Engine::Component::Component, T>::value, "T must inherit from Engine::Component::Component");
        auto it = pools.find(std::type_index(typeid(T)));
        if (it == pools.end()) return nullptr;

        return static_cast<SparseSet<T>*>(it->second.get())->try_Get(entity);
    }
    template<typename T>
    bool Has_component(Entity entity) {
        static_assert(std::is_base_of<Engine::Component::Component, T>::value, "T must inherit from Engine::Component::Component");
        auto it = pools.find(std::type_index(typeid(T)));
        if (it == pools.end()) return false;

        return static_cast<SparseSet<T>*>(it->second.get())->has(entity);
    }
    template<typename T>
    bool remove_component(Entity entity) {
        static_assert(std::is_base_of<Engine::Component::Component, T>::value, "T must inherit from Engine::Component::Component");
        auto it = pools.find(std::type_index(typeid(T)));
        if (it == pools.end()) {
            assert(true && "Deleting a component from an have fail");
            return false;
        }
        return it->second.get()->Remove(entity);
    }

    template<typename T>
    T* get_ressource() {
        static_assert(std::is_base_of<Engine::Resource::Resource, T>::value, "T must inherit from Engine::Resource::Resource");
        auto type_id = std::type_index(typeid(T));

        if (ressources.find(type_id) == ressources.end()) {
            std::cout << "The resource: " << typeid(T).name() << " does not exist" << std::endl;
            return nullptr;
        }

        // Cast
        return static_cast<T*>(ressources[type_id].get());
    }

    AssetStore* assetStore;
    Renderer* renderer;
private:
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> pools;
    std::unordered_map<std::type_index, std::unique_ptr<Engine::Resource::Resource>> ressources;
    std::deque<Entity> entity_emptySlot;
    uint32_t entity_Register = 0;

    // Recupére/crée un pool spécifique
    template<typename T>
    SparseSet<T>* get_pool() {
        auto type_id = std::type_index(typeid(T));

        if (pools.find(type_id) == pools.end()) {
            pools.emplace(type_id, std::make_unique<SparseSet<T>>());
        }

        // Cast
        return static_cast<SparseSet<T>*>(pools[type_id].get());
    }

    template<typename T>
    T* add_ressource() {
        static_assert(std::is_base_of<Engine::Resource::Resource, T>::value, "T must inherit from Engine::Resource::Resource");
        auto type_id = std::type_index(typeid(T));

        if (ressources.find(type_id) == ressources.end()) {
            ressources.emplace(type_id, std::make_unique<T>());
        }

        // Cast
        return static_cast<T*>(ressources[type_id].get());
    }

    friend class Scheduler;
};