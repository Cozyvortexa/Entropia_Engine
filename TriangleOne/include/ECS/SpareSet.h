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

    void insert(Entity e, const T& component) {
        if (!contains(e)) {
            // Redimensionner le tableau sparse si l'ID de l'entité est plus grand que la taille interne du tableau
            if (e >= sparse.size()) {
                sparse.resize(e + 2000, static_cast<size_t>(NULL_ENTITY));
            }

            sparse[e] = dense_entities.size();

            // Ajout de l'entité aux tableaux denses
            dense_entities.push_back(e);
            dense_components.push_back(component);
        }
        else {
            // Met à jour le composant si l'entité l'a déjà
            dense_components[sparse[e]] = component;
        }
    }
    bool has(int entity_id) const {
        return entity_id < sparse.size() && sparse[entity_id] != NULL_ENTITY;
    }

    // Swap and Pop
    bool Remove(Entity e) override {
        if (!contains(e)) return false;

        size_t deleted_idx = sparse[e];
        size_t last_idx = dense_entities.size() - 1;

        // Remplace l'élément supprimé par le dernier élément du tableau dense
        Entity last_entity = dense_entities[last_idx];

        dense_entities[deleted_idx] = dense_entities[last_idx];
        dense_components[deleted_idx] = dense_components[last_idx];

        // Met à jour l'index de la dernière entité dans le tableau sparse
        sparse[last_entity] = deleted_idx;

        // Invalide l'entité supprimée
        sparse[e] = static_cast<size_t>(NULL_ENTITY);

        // Retire le dernier élément
        dense_entities.pop_back();
        dense_components.pop_back();

        return true;
    }

    // Récupère une référence vers le composant d'une entité
    T& get(Entity e) {
        assert(contains(e) && "L'entite ne possede pas ce composant !");
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