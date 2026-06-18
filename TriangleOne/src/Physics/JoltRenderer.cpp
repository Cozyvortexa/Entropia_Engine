#include "Physics/JoltRenderer.h"

#include "ECS/World.h"

namespace Component = Engine::Component;

namespace Engine::Physics {
    void JoltDebugRenderer::DisplayDebugShape(World* world, JPH::Color debugColor ) {
        for (const auto& currentEntity : debugShapeList) {
            Component::Transform* transform = world->get_component<Component::Transform>(currentEntity.first);
            Component::PhysicObject* physicObject = world->get_component_of_type<Component::PhysicObject, Component::BoxCollider, Component::SphereCollider>(currentEntity.first);

            if (physicObject != nullptr && physicObject->displayShape) {
                JPH::RMat44 globalTransform = Physics::PhysicsHelper::To_RMat44_JPH(transform->position, transform->rotation);

                Physics::PhysicsHelper::DrawShape(*physicObject, this, globalTransform, debugColor);
            }

        }
        debugShapeList.clear();
    }
}