#include "Physics/PhysicsHelper.h"

#include "ECS/Components/PhysicComponent.h"

#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Utilities/CoACD/public/coacd.h>


namespace Physics = Engine::Physics;


void Physics::PhysicsHelper::Init(JPH::PhysicsSystem* set_physics_system) {
    assert(set_physics_system != nullptr);

    physics_system = set_physics_system;
    body_interface = &physics_system->GetBodyInterface();
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

    return glm::eulerAngles(glmQuat);
}

glm::vec3 Physics::PhysicsHelper::JoltQuatToEulerDegrees(const JPH::Quat& q) {
    glm::quat glmQuat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());

    glm::vec3 euler = glm::eulerAngles(glmQuat);

    return glm::degrees(euler);
}

JPH::Quat Physics::PhysicsHelper::To_JoltQuat(const glm::vec3& eulerDegrees){
    glm::vec3 rad = glm::radians(eulerDegrees);
    return JPH::Quat::sEulerAngles(JPH::Vec3(rad.x, rad.y, rad.z));
}

JPH::RMat44 Physics::PhysicsHelper::To_RMat44_JPH(const glm::vec3& position, const glm::vec3& rotation) {
    JPH::RVec3 jphPosition = To_Rvec3_JPH(position);
    JPH::Quat jphRotation = To_JoltQuat(rotation);

    return JPH::RMat44::sRotationTranslation(jphRotation, jphPosition);
}

#pragma endregion

#pragma region Create Shape

JPH::BodyID Physics::PhysicsHelper::CreateBody_With_Param(JPH::ConvexShapeSettings& shape_settings, glm::vec3 position,
    JPH::Quat quaternion = JPH::Quat::sIdentity(), JPH::EMotionType motionType = JPH::EMotionType::Static, JPH::ObjectLayer layers = Layers::MOVING) {

    JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
    JPH::ShapeRefC shape = shape_result.Get();

#ifndef NDEBUG
    if (shape_result.HasError())std::cout << shape_result.GetError() << std::endl;
#endif

    JPH::BodyCreationSettings settings(shape, To_Rvec3_JPH(position), quaternion, motionType, layers);
    if (motionType == JPH::EMotionType::Dynamic || motionType == JPH::EMotionType::Kinematic) settings.mAllowDynamicOrKinematic = true;

    return body_interface->CreateBody(settings)->GetID();
}

JPH::BodyID Physics::PhysicsHelper::CreateBox(const glm::vec3 position, glm::vec3 size, const JPH::Quat quaternion, JPH::EMotionType motionType) {
    JPH::BoxShapeSettings shape_settings(To_Vec3_JPH(size));
    shape_settings.SetEmbedded();

    return CreateBody_With_Param(shape_settings, position, quaternion, motionType);
}

JPH::BodyID Physics::PhysicsHelper::CreateSphere(const glm::vec3 position, float radius, const JPH::Quat quaternion, JPH::EMotionType motionType) {
    JPH::SphereShapeSettings shape_settings(radius);
    shape_settings.SetEmbedded();

    return CreateBody_With_Param(shape_settings, position, quaternion, motionType);
}


JPH::BodyID Physics::PhysicsHelper::CreateConvexShape(const JPH::Array<JPH::Vec3>& vertices_Position, glm::vec3 position,
    JPH::Quat quaternion = JPH::Quat::sIdentity(), JPH::EMotionType motionType = JPH::EMotionType::Static, JPH::ObjectLayer layers = Layers::MOVING) {

    JPH::ConvexHullShapeSettings hullSettings(vertices_Position);
    JPH::ShapeRefC shape = hullSettings.Create().Get();


    JPH::BodyCreationSettings settings(shape, To_Rvec3_JPH(position), quaternion, motionType, layers);
    if (motionType == JPH::EMotionType::Dynamic || motionType == JPH::EMotionType::Kinematic) settings.mAllowDynamicOrKinematic = true;

    return body_interface->CreateBody(settings)->GetID();
}

#pragma endregion

#pragma region Simulated World Interaction
void Physics::PhysicsHelper::AddBody_To_SimulateWorld(const Engine::Component::PhysicObject& physicObject) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    body_interface->AddBody(physicObject.bodyID, JPH::EActivation::Activate);
}

void Physics::PhysicsHelper::RemoveBody_From_SimulateWorld(const Engine::Component::PhysicObject& physicObject) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    if (body_interface->IsAdded(physicObject.bodyID)) {
        body_interface->RemoveBody(physicObject.bodyID);
    }
}

void Physics::PhysicsHelper::DeleteBody(const Engine::Component::PhysicObject& physicObject) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    if (body_interface->IsAdded(physicObject.bodyID)) body_interface->RemoveBody(physicObject.bodyID);
    body_interface->DestroyBody(physicObject.bodyID);
}

void Physics::PhysicsHelper::Impulse(const Engine::Component::PhysicObject& physicObject, glm::vec3 force) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    JPH::BodyLockWrite lock(physics_system->GetBodyLockInterface(), physicObject.bodyID);
    if (lock.Succeeded()) {
        JPH::Body& body = lock.GetBody();
        body.AddImpulse(To_Vec3_JPH(force));
    }
}

void Physics::PhysicsHelper::AngularImpulse(const Engine::Component::PhysicObject& physicObject, glm::vec3 force) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    JPH::BodyLockWrite lock(physics_system->GetBodyLockInterface(), physicObject.bodyID);
    if (lock.Succeeded()) {
        JPH::Body& body = lock.GetBody();
        body.AddAngularImpulse(To_Vec3_JPH(force));
    }
}

#pragma endregion 

#pragma region Setter

void Physics::PhysicsHelper::Set_MotionType(const Engine::Component::PhysicObject& physicObject, JPH::EMotionType newMotionType) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    body_interface->SetMotionType(physicObject.bodyID, newMotionType, body_interface->IsActive(physicObject.bodyID) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
}

void Physics::PhysicsHelper::Set_Layer(const Engine::Component::PhysicObject& physicObject, JPH::ObjectLayer newLayers) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    body_interface->SetObjectLayer(physicObject.bodyID, newLayers);
}

void Physics::PhysicsHelper::Set_Gravity(const Engine::Component::PhysicObject& physicObject, float newValue) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    body_interface->SetGravityFactor(physicObject.bodyID, newValue);
    if (newValue > 0 && !body_interface->IsActive(physicObject.bodyID)) body_interface->ActivateBody(physicObject.bodyID);
}

void Physics::PhysicsHelper::Set_Position(const Engine::Component::PhysicObject& physicObject, glm::vec3 position) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) glm::vec3(-1);

    body_interface->SetPosition(physicObject.bodyID, To_Vec3_JPH(position), body_interface->IsActive(physicObject.bodyID) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
}

void Physics::PhysicsHelper::Set_Rotation(const Engine::Component::PhysicObject& physicObject, glm::vec3 rotation) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) glm::vec3(-1);

    body_interface->SetRotation(physicObject.bodyID, To_JoltQuat(rotation), body_interface->IsActive(physicObject.bodyID) ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
}

void Physics::PhysicsHelper::Set_Restitution(const Engine::Component::PhysicObject& physicObject, float newValue) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    body_interface->SetRestitution(physicObject.bodyID, newValue);
}

void Physics::PhysicsHelper::Set_Friction(const Engine::Component::PhysicObject& physicObject, float newValue) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    body_interface->SetFriction(physicObject.bodyID, newValue);
}

void Physics::PhysicsHelper::Set_LinearDamping(const Engine::Component::PhysicObject& physicObject, float newValue) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    JPH::BodyLockWrite lock(physics_system->GetBodyLockInterface(), physicObject.bodyID);
    if (lock.Succeeded()){
        JPH::Body& body = lock.GetBody();
        body.GetMotionProperties()->SetLinearDamping(newValue);
    }
}

void Physics::PhysicsHelper::Set_AngularDamping(const Engine::Component::PhysicObject& physicObject, float newValue) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;

    JPH::BodyLockWrite lock(physics_system->GetBodyLockInterface(), physicObject.bodyID);
    if (lock.Succeeded()){
        JPH::Body& body = lock.GetBody();
        body.GetMotionProperties()->SetAngularDamping(newValue);
    }
}

#pragma endregion

#pragma region Getter

glm::vec3 Physics::PhysicsHelper::Get_Position(const Engine::Component::PhysicObject& physicObject) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) glm::vec3(-1);

    return To_Vec3_GLM(body_interface->GetPosition(physicObject.bodyID));
}

glm::vec3 Physics::PhysicsHelper::Get_CenterOfMass(const Engine::Component::PhysicObject& physicObject) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) glm::vec3(-1);

    return To_Vec3_GLM(body_interface->GetCenterOfMassPosition(physicObject.bodyID));
}

glm::vec3 Physics::PhysicsHelper::Get_EulerRotation(const Engine::Component::PhysicObject& physicObject) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) glm::vec3(-1);

    return JoltQuatToEulerDegrees(body_interface->GetRotation(physicObject.bodyID));
}

JPH::Quat Physics::PhysicsHelper::Get_QuatRotation(const Engine::Component::PhysicObject& physicObject) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) glm::vec3(-1);

    return body_interface->GetRotation(physicObject.bodyID);
}

JPH::EMotionType Physics::PhysicsHelper::Get_MotionType(const Engine::Component::PhysicObject& physicObject) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) glm::vec3(-1);

    return body_interface->GetMotionType(physicObject.bodyID);
}

float Physics::PhysicsHelper::Get_LinearVelocity(const Engine::Component::PhysicObject& physicObject) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return -1;

    JPH::BodyLockWrite lock(physics_system->GetBodyLockInterface(), physicObject.bodyID);
    if (lock.Succeeded()) {
        JPH::Body& body = lock.GetBody();
        return body.GetMotionProperties()->GetLinearDamping();
    }
    return -2;
}

#pragma endregion

#pragma region Resize shape
JPH::BodyID Physics::PhysicsHelper::ResizeBox(const Engine::Component::PhysicObject& physicObject, glm::vec3 newSize) {
    glm::vec3 position = Get_Position(physicObject);
    JPH::Quat quat = Get_QuatRotation(physicObject);
    JPH::EMotionType motionType = Get_MotionType(physicObject);

    DeleteBody(physicObject);
    return CreateBox(position, newSize, quat, motionType);
}

JPH::BodyID Physics::PhysicsHelper::ResizeSphere(const Engine::Component::PhysicObject& physicObject, float newRadius) {
    glm::vec3 position = Get_Position(physicObject);
    JPH::Quat quat = Get_QuatRotation(physicObject);
    JPH::EMotionType motionType = Get_MotionType(physicObject);

    DeleteBody(physicObject);
    return CreateSphere(position, newRadius, quat, motionType);
}

#pragma endregion


void Physics::PhysicsHelper::DrawShape(const Engine::Component::PhysicObject& physicObject, JPH::DebugRenderer* debugRenderer, JPH::RMat44Arg center, JPH::ColorArg debugColor) {
    assert(!physicObject.bodyID.IsInvalid());
    if (physicObject.bodyID.IsInvalid()) return;


    JPH::BodyLockWrite lock(physics_system->GetBodyLockInterface(), physicObject.bodyID);
    if (lock.Succeeded()) {
        JPH::Body& body = lock.GetBody();
        const JPH::Shape* shape = body.GetShape();
        if (shape != nullptr) {
            shape->Draw(debugRenderer, center, JPH::Vec3::sReplicate(1.0f), debugColor, false, true );
        }
    }
}


JPH::BodyID Physics::PhysicsHelper::CreateCompoundConvexShape(
    const std::vector<JPH::Array<JPH::Vec3>>& hullsVertices, glm::vec3 position, JPH::Quat quaternion = JPH::Quat::sIdentity(),
    JPH::EMotionType motionType = JPH::EMotionType::Static, JPH::ObjectLayer layers = Layers::MOVING){

    JPH::Ref<JPH::StaticCompoundShapeSettings> compoundSettings = new JPH::StaticCompoundShapeSettings();

    for (const JPH::Array<JPH::Vec3>& hullVertices : hullsVertices) {
        JPH::ConvexHullShapeSettings hullSettings(hullVertices);
        // Chaque hull est déjà positionné correctement les uns par rapport aux autres
        // (coordonnées relatives issues de la décomposition) -> offset local nul
        //compoundSettings->AddShape(JPH::Vec3::sZero(), JPH::Quat::sIdentity(), hullSettings);
    }

    JPH::ShapeRefC shape = compoundSettings->Create().Get();
    JPH::BodyCreationSettings settings(shape, To_Rvec3_JPH(position), quaternion, motionType, layers);
    if (motionType == JPH::EMotionType::Dynamic || motionType == JPH::EMotionType::Kinematic)
        settings.mAllowDynamicOrKinematic = true;

    return body_interface->CreateBody(settings)->GetID();
}