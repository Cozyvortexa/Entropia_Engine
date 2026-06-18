#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <tuple>

#include <iostream>
#include <vector>
#include <deque>
#include "ECS/SpareSet.h"
#include "ECS/Components/ComponentBase.h"
#include "ECS/Resources/Resource.h"
#include "ECS/AssetStore.h"

#include "Render/Renderer.h"
#include "Audio/AudioHelper.h"

#include <unordered_map>
#include <typeindex>
#include <cassert>
#include <algorithm>
#include <memory>
#include <type_traits>


struct ComponentTypeCounter {
    static inline std::size_t s_next = 0;
};
struct ResourceTypeCounter {
    static inline std::size_t s_next = 0;
};

template<typename T>
std::size_t GetComponentTypeID() {
    static const std::size_t id = ComponentTypeCounter::s_next++;
    return id;
}
template<typename T>
std::size_t GetResourceTypeID() {
    static const std::size_t id = ResourceTypeCounter::s_next++;
    return id;
}

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
        for (int i = 0; i < pools.size(); i++) {
            pools[i].get()->Remove(entity);
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

        std::size_t id = GetComponentTypeID<T>();
        if (id >= pools.size() || !pools[id]) return nullptr;

        return static_cast<SparseSet<T>*>(pools[id].get())->try_Get(entity);
    }
    template <typename Parent, typename... Components>
    Parent* get_component_of_type(Entity entity) {
        Parent* result = nullptr;

        // Iterate over each component
        ((result = [](World* world, Entity entity) -> Parent* {
            auto* comp = world->get_component<Components>(entity);
            if (comp != nullptr) {
                return static_cast<Parent*>(comp);
            }
            return nullptr;
            }(this, entity)) || ...); // "|| ..." = Stop the search as soon as a valid pointer is found

        return result;
    }
    template<typename T>
    bool Has_component(Entity entity) {
        static_assert(std::is_base_of<Engine::Component::Component, T>::value, "T must inherit from Engine::Component::Component");

        std::size_t id = GetComponentTypeID<T>();
        if (id >= pools.size() || !pools[id]) return false;

        return static_cast<SparseSet<T>*>(pools[id].get())->has(entity);
    }
    template<typename T>
    bool remove_component(Entity entity) {
        static_assert(std::is_base_of<Engine::Component::Component, T>::value, "T must inherit from Engine::Component::Component");

        std::size_t id = GetComponentTypeID<T>();
        if (id >= pools.size() || !pools[id]) return false;

        return pools[id].get()->Remove(entity);
    }

    template<typename T>
    T* get_ressource() {
        static_assert(std::is_base_of<Engine::Resource::Resource, T>::value, "T must inherit from Engine::Resource::Resource");
        std::size_t id = GetResourceTypeID<T>();

        if (id >= resources.size() || !resources[id]) {
            std::cout << "The resource: " << typeid(T).name() << " does not exist" << std::endl;
            return nullptr;
        }

        return static_cast<T*>(resources[id].get());
    }

    AssetStore* assetStore = nullptr;
    Renderer* renderer = nullptr;
private:
    std::vector<std::unique_ptr<ISparseSet>> pools;
    std::vector<std::unique_ptr<Engine::Resource::Resource>> resources;

    std::deque<Entity> entity_emptySlot;
    uint32_t entity_Register = 0;

    // Retrieve/create a specific pool
    template<typename T>
    SparseSet<T>* get_pool() {
        std::size_t id = GetComponentTypeID<T>();
        if (id >= pools.size()) pools.resize(id + 1);

        if (!pools[id]) pools[id] = std::make_unique<SparseSet<T>>();
        return static_cast<SparseSet<T>*>(pools[id].get());
    }

    template<typename T>
    T* add_ressource() {
        static_assert(std::is_base_of<Engine::Resource::Resource, T>::value, "T must inherit from Engine::Resource::Resource");

        std::size_t id = GetResourceTypeID<T>();
        if (id >= resources.size()) resources.resize(id + 1);

        if (!resources[id]) resources[id] = std::make_unique<T>();
        return static_cast<T*>(resources[id].get());
    }

    friend class Scheduler;
};