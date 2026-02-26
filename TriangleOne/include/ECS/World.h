#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <window.h>
#include <tuple>

#include <iostream>
#include <vector>
#include "ECS/SpareSet.h"
#include "ECS/Component.h"

#include <unordered_map>
#include <typeindex>
#include <cassert>
#include <algorithm>
#include <memory>



template<typename... Components>
class View {
private:
    std::tuple<SparseSet<Components>*...> pools;

public:
    View(SparseSet<Components>*... ptrs) : pools(ptrs...) {}

    template<typename Func>
    void each(Func callback) {
        auto* lead_pool = std::get<0>(pools);  // WARNING: Recupére le premier pool donné en paramètre

        for (size_t i = 0; i < lead_pool->dense_entities.size(); ++i) {
            Entity entity = lead_pool->dense_entities[i];

            if ((std::get<SparseSet<Components>*>(pools)->has(entity) && ...)) {
                callback(entity, std::get<SparseSet<Components>*>(pools)->get(entity)...);
            }
        }
    }
};

class World {
public:
    template<typename... Components>
    View<Components...> view() {
        return View<Components...>(get_pool<Components>()...);
    }

    template<typename T>
    void add_component(int entity, T component) {
        get_pool<T>()->insert(entity, component);
    }
    template<typename T>
    T* get_component(Entity entity) {
        auto it = pools.find(std::type_index(typeid(T)));
        if (it == pools.end()) return nullptr;

        return static_cast<SparseSet<T>*>(it->second.get())->try_get(entity);
    }
    template<typename T>
    bool remove_component(Entity entity) {
        auto it = pools.find(std::type_index(typeid(T)));
        if (it == pools.end()) {
            assert(true, "La supression d'un composant sur l'entité numéro: " + entity + " à échouer");
            return false
        }
        return static_cast<SparseSet<T>*>(it->second.get())->remove(entity);
    }

    template<typename T>
    T* get_ressource() {
        auto type_id = std::type_index(typeid(T));

        if (ressources.find(type_id) == ressources.end()) {
            std::cout << "La ressource: " << typeid(T).name() << " n'existe pas" << std::endl;
            return nullptr;
        }

        // Cast
        return static_cast<T*>(ressources[type_id].get());
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> pools;
    std::unordered_map<std::type_index, std::unique_ptr<Resource>> ressources;

    // Recupére/crée un pool spécifique
    template<typename T>
    SparseSet<T>* get_pool() {
        auto type_id = std::type_index(typeid(T));

        if (pools.find(type_id) == pools.end()) {
            pools[type_id] = std::make_unique<SparseSet<T>>();
        }

        // Cast
        return static_cast<SparseSet<T>*>(pools[type_id].get());
    }

    template<typename T>
    T* add_ressource() {
        auto type_id = std::type_index(typeid(T));

        if (ressources.find(type_id) == ressources.end()) {
            ressources[type_id] = std::make_unique<T>();
        }

        // Cast
        return static_cast<T*>(ressources[type_id].get());
    }

    friend class Scheduler;
};