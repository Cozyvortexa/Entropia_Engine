#include "Physics/JoltRenderer.h"

#include "ECS/World.h"

namespace Engine::Physics {
    void JoltDebugRenderer::DisplayDebugShape(World* world, JPH::Color debugColor ) {
        for (const auto& currentEntity : debugShapeList) {
            Engine::Component::Transform* transform = world->get_component<Engine::Component::Transform>(currentEntity.first);

            if (currentEntity.second == Physics::ShapeType::Box) {
                Engine::Component::BoxCollider* boxCollider = world->get_component<Engine::Component::BoxCollider>(currentEntity.first);
                if (boxCollider != nullptr && boxCollider->displayShape) {
                    JPH::RMat44 globalTransform = Physics::PhysicsHelper::To_RMat44_JPH(transform->position, transform->rotation);

                    Physics::PhysicsHelper::DrawShape(*boxCollider, this, globalTransform, debugColor);
                }
            }
            else if (currentEntity.second == Physics::ShapeType::Sphere) {
                Engine::Component::SphereCollider* sphereCollider = world->get_component<Engine::Component::SphereCollider>(currentEntity.first);
                if (sphereCollider != nullptr && sphereCollider->displayShape) {
                    JPH::RMat44 globalTransform = Physics::PhysicsHelper::To_RMat44_JPH(transform->position, transform->rotation);

                    Physics::PhysicsHelper::DrawShape(*sphereCollider, this, globalTransform, debugColor);
                }
            }
        }
        debugShapeList.clear();
    }
}