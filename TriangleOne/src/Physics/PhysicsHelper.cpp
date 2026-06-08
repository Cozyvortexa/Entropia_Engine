#include "Physics/PhysicsHelper.h"

namespace Physics = Engine::Physics;


void Physics::PhysicsHelper::Init(JPH::BodyInterface& new_body_interface) {
    //assert(&new_body_interface == nullptr); //In case
    body_interface = &new_body_interface;
}

//JPH::BodyInterface* Physics::PhysicsHelper::GetBodyInterface() {
//    assert(body_interface && "BodyInterface is null");
//    return body_interface.get();
//}

#pragma region Converter
JPH::Vec3 Physics::PhysicsHelper::To_Vec3_JPH(const glm::vec3 vec) {
    return JPH::Vec3(vec.x, vec.y, vec.z);
}
JPH::RVec3 Physics::PhysicsHelper::To_Rvec3_JPH(const glm::vec3 vec) {
    return JPH::RVec3(vec.x, vec.y, vec.z);
}
glm::vec3 Physics::PhysicsHelper::To_Vec3_GLM(const JPH::Vec3& v){
    return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
}

glm::vec3 Physics::PhysicsHelper::JoltQuatToEuler(const JPH::Quat& q) {
    glm::quat glmQuat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());

    // Résultat en radians
    return glm::eulerAngles(glmQuat);
}
glm::vec3 Physics::PhysicsHelper::JoltQuatToEulerDegrees(const JPH::Quat& q) {
    glm::quat glmQuat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());

    glm::vec3 euler = glm::eulerAngles(glmQuat);

    return glm::degrees(euler);
}

#pragma endregion

#pragma region Create Shape
JPH::BodyID Physics::PhysicsHelper::CreateBody_With_Param(JPH::BoxShapeSettings& shape_settings, glm::vec3 position) {
    JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
    JPH::ShapeRefC shape = shape_result.Get();

    #ifndef NDEBUG
    if (shape_result.HasError())std::cout << shape_result.GetError() << std::endl;
    #endif

    JPH::BodyCreationSettings settings(shape, To_Rvec3_JPH(position), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::MOVING);
    return body_interface->CreateBody(settings)->GetID();
}
JPH::BodyID Physics::PhysicsHelper::CreateBody_With_Param(JPH::BoxShapeSettings& shape_settings, glm::vec3 position, JPH::Quat quaternion) {
    JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
    JPH::ShapeRefC shape = shape_result.Get();

    #ifndef NDEBUG
    if (shape_result.HasError())std::cout << shape_result.GetError() << std::endl;
    #endif

    JPH::BodyCreationSettings settings(shape, To_Rvec3_JPH(position), quaternion, JPH::EMotionType::Static, Layers::MOVING);
    return body_interface->CreateBody(settings)->GetID(); //mAllowDynamicOrKinematic set to true
}
JPH::BodyID Physics::PhysicsHelper::CreateBody_With_Param(JPH::BoxShapeSettings& shape_settings, glm::vec3 position, JPH::Quat quaternion, JPH::EMotionType motionType) {
    JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
    JPH::ShapeRefC shape = shape_result.Get();

    #ifndef NDEBUG
    if (shape_result.HasError())std::cout << shape_result.GetError() << std::endl;
    #endif

    JPH::BodyCreationSettings settings(shape, To_Rvec3_JPH(position), quaternion, motionType, Layers::MOVING);
    return body_interface->CreateBody(settings)->GetID();
}
JPH::BodyID Physics::PhysicsHelper::CreateBody_With_Param(JPH::BoxShapeSettings& shape_settings, glm::vec3 position, JPH::Quat quaternion, JPH::EMotionType motionType, JPH::ObjectLayer layers) {
    JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
    JPH::ShapeRefC shape = shape_result.Get();

#ifndef NDEBUG
    if (shape_result.HasError())std::cout << shape_result.GetError() << std::endl;
#endif

    JPH::BodyCreationSettings settings(shape, To_Rvec3_JPH(position), quaternion, motionType, layers);
    return body_interface->CreateBody(settings)->GetID();
}


JPH::BodyID Physics::PhysicsHelper::CreateBox(const glm::vec3 position) {
    JPH::BoxShapeSettings shape_settings(JPH::Vec3(1.0f, 1.0f, 1.0f));
    shape_settings.SetEmbedded();

    return CreateBody_With_Param(shape_settings, position);
}
JPH::BodyID Physics::PhysicsHelper::CreateBox(const glm::vec3 position, glm::vec3 size) {
    JPH::BoxShapeSettings shape_settings(To_Vec3_JPH(size));
    shape_settings.SetEmbedded();

    return CreateBody_With_Param(shape_settings, position);
}
JPH::BodyID Physics::PhysicsHelper::CreateBox(const glm::vec3 position, glm::vec3 size, JPH::Quat quaternion) {
    JPH::BoxShapeSettings shape_settings(To_Vec3_JPH(size));
    shape_settings.SetEmbedded();

    return CreateBody_With_Param(shape_settings, position, quaternion);
}
JPH::BodyID Physics::PhysicsHelper::CreateBox(const glm::vec3 position, glm::vec3 size, JPH::Quat quaternion, JPH::EMotionType motionType) {
    JPH::BoxShapeSettings shape_settings(To_Vec3_JPH(size));
    shape_settings.SetEmbedded();

    return CreateBody_With_Param(shape_settings, position, quaternion, motionType);
}

#pragma endregion

#pragma region Simulated World Interaction
void Physics::PhysicsHelper::AddBody_To_SimulateWorld(const JPH::BodyID& bodyID) {
    assert(!bodyID.IsInvalid());
    if (bodyID.IsInvalid()) return;

    body_interface->AddBody(bodyID, JPH::EActivation::Activate);
}

void Physics::PhysicsHelper::RemoveBody_From_SimulateWorld(const JPH::BodyID& bodyID) {
    assert(!bodyID.IsInvalid());
    if (bodyID.IsInvalid()) return;

    if (body_interface->IsAdded(bodyID)) {
        body_interface->RemoveBody(bodyID);
    }
}

void Physics::PhysicsHelper::DeleteBody(const JPH::BodyID& bodyID) {
    assert(!bodyID.IsInvalid());
    if (bodyID.IsInvalid()) return;

    if (body_interface->IsAdded(bodyID)) body_interface->RemoveBody(bodyID);
    body_interface->DestroyBody(bodyID);
}

#pragma endregion 

#pragma region Setter

void Physics::PhysicsHelper::SetMotionType(const JPH::BodyID& bodyID, JPH::EMotionType newMotionType) {
    assert(!bodyID.IsInvalid());
    if (bodyID.IsInvalid()) return;

    body_interface->SetMotionType(bodyID, newMotionType, body_interface->IsActive(bodyID) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
}

void Physics::PhysicsHelper::SetLayer(const JPH::BodyID& bodyID, JPH::ObjectLayer newLayers) {
    assert(!bodyID.IsInvalid());
    if (bodyID.IsInvalid()) return;

    body_interface->SetObjectLayer(bodyID, newLayers);
}

void Physics::PhysicsHelper::SetGravity(const JPH::BodyID& bodyID, float newValue) {
    assert(!bodyID.IsInvalid());
    if (bodyID.IsInvalid()) return;

    body_interface->SetGravityFactor(bodyID, newValue);
}


void Physics::PhysicsHelper::SetPosition(const JPH::BodyID& bodyID, glm::vec3 position) {
    assert(!bodyID.IsInvalid());
    if (bodyID.IsInvalid()) glm::vec3(-1);

    body_interface->SetPosition(bodyID, To_Vec3_JPH(position), body_interface->IsActive(bodyID) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
}


#pragma endregion

#pragma region Getter

glm::vec3 Physics::PhysicsHelper::GetPosition(const JPH::BodyID& bodyID) {
    assert(!bodyID.IsInvalid());
    if (bodyID.IsInvalid()) glm::vec3(-1);

    return To_Vec3_GLM(body_interface->GetPosition(bodyID));
}

glm::vec3 Physics::PhysicsHelper::GetCenterOfMass(const JPH::BodyID& bodyID) {
    assert(!bodyID.IsInvalid());
    if (bodyID.IsInvalid()) glm::vec3(-1);

    return To_Vec3_GLM(body_interface->GetCenterOfMassPosition(bodyID));
}

glm::vec3 Physics::PhysicsHelper::GetRotation(const JPH::BodyID& bodyID) {
    assert(!bodyID.IsInvalid());
    if (bodyID.IsInvalid()) glm::vec3(-1);

    return JoltQuatToEulerDegrees(body_interface->GetRotation(bodyID));
}
#pragma endregion

#pragma region Resize shape
JPH::BodyID Physics::PhysicsHelper::ResizeBox(const JPH::BodyID& bodyID, glm::vec3 newSize) {
    DeleteBody(bodyID);
    return CreateBox(newSize);
}

#pragma endregion