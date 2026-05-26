#pragma once
#include <vector>
#include <cassert>
#include <cstdint>
#include <iostream>

using Entity = uint32_t;
const Entity NULL_ENTITY = static_cast<Entity>(-1);

class ISparseSet {
public:
    virtual ~ISparseSet() = default;
    virtual bool Remove(Entity entity_id) = 0;
};

template <typename T>
class SparseSet : public ISparseSet {
public:
    std::vector<size_t> sparse;
    std::vector<Entity> dense_entities;
    std::vector<T> dense_components;


    bool contains(Entity e) const {
        return e < sparse.size() &&
            sparse[e] < dense_entities.size() &&
            dense_entities[sparse[e]] == e;
    }
    //for Lvalue
    void insert(Entity e, const T& component) {
        if (!contains(e)) {
            // Resize the sparse array if the entity ID exceeds the array size
            if (e >= sparse.size()) {
                sparse.resize(e + 2000, static_cast<size_t>(NULL_ENTITY));
            }

            sparse[e] = dense_entities.size();

            // Add the entity to the dense arrays
            dense_entities.push_back(e);
            dense_components.push_back(component);
        }
        else {
            // Update the component if the entity already has it
            dense_components[sparse[e]] = component;
        }
    }
    //for Rvalue
    void insert(Entity e, T&& component) {
        if (!contains(e)) {
            if (e >= sparse.size()) {
                sparse.resize(e + 2000, static_cast<size_t>(NULL_ENTITY));
            }
            sparse[e] = dense_entities.size();
            dense_entities.push_back(e);

            dense_components.push_back(std::move(component));
        }
        else {
            dense_components[sparse[e]] = std::move(component);
        }
    }

    bool has(int entity_id) const {
        return entity_id < sparse.size() && sparse[entity_id] != NULL_ENTITY;
    }

    // Swap and Pop
    bool Remove(Entity e) override {
        //Remove doesn't use contains to avoid a redundant access to the sparse array
        if (e >= sparse.size()) return false; 

        size_t deleted_idx = sparse[e];
        if (deleted_idx >= dense_entities.size() || dense_entities[deleted_idx] != e) return false;

        size_t last_idx = dense_entities.size() - 1;


        if (deleted_idx != last_idx) {
            // Swap the removed element with the last element in the dense arrays
            Entity last_entity = dense_entities[last_idx];

            dense_entities[deleted_idx] = std::move(dense_entities[last_idx]);
            dense_components[deleted_idx] = std::move(dense_components[last_idx]);

            // Update the sparse index of the last entity
            sparse[last_entity] = deleted_idx;
        }

        // Invalidate the removed entity
        sparse[e] = static_cast<size_t>(NULL_ENTITY);

        // Remove the last element
        dense_entities.pop_back();
        dense_components.pop_back();

        return true;
    }

    T& get(Entity e) {
        assert(contains(e) && "The entity does not have this component!");
        return dense_components[sparse[e]];
    }

    T* try_Get(Entity e) {
        if (contains(e)) {
            return &dense_components[sparse[e]];
        }
        return nullptr;
    }

    size_t size() const { return dense_components.size(); }

    T* data() { return dense_components.data(); }

    auto begin() { return dense_components.begin(); }
    auto end() { return dense_components.end(); }
};