#pragma once
#include "Physics/PhysicsStruct.h"
#include <assert.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine::Component {
    class PhysicObject;
}

namespace Engine::Physics {
    class PhysicsHelper {
    public:
        PhysicsHelper() = delete;
        static void Init(JPH::PhysicsSystem* set_physics_system);
        //static JPH::BodyInterface& GetBodyInterface();

    #pragma region Converter
        static inline JPH::Vec3 To_Vec3_JPH(const glm::vec3 vec);
        static inline JPH::RVec3 To_Rvec3_JPH(const glm::vec3 vec);
        static inline glm::vec3 To_Vec3_GLM(const JPH::Vec3& v);

        static inline glm::vec3 JoltQuatToEuler(const JPH::Quat& q);
        static inline glm::vec3 JoltQuatToEulerDegrees(const JPH::Quat& q);

        static inline JPH::Quat To_JoltQuat(const glm::vec3& eulerDegrees);

        static JPH::RMat44 To_RMat44_JPH(const glm::vec3& position, const glm::vec3& rotation);

    #pragma endregion

    #pragma region Create Shape
        static JPH::BodyID CreateBody_With_Param(JPH::ConvexShapeSettings& shape_settings, glm::vec3 position, JPH::Quat quaternion, JPH::EMotionType motionType, JPH::ObjectLayer layers);

        static JPH::BodyID CreateBox(const glm::vec3 position, glm::vec3 size, const JPH::Quat quaternion, JPH::EMotionType motionType);

        static JPH::BodyID CreateSphere(const glm::vec3 position, float radius, const JPH::Quat quaternion, JPH::EMotionType motionType);

        static JPH::BodyID CreateConvexShape(const JPH::Array<JPH::Vec3>& vertices_Position, glm::vec3 position, JPH::Quat quaternion, JPH::EMotionType motionType, JPH::ObjectLayer layers);

    #pragma endregion

    #pragma region Simulated World Interaction
        static void AddBody_To_SimulateWorld(const Engine::Component::PhysicObject& physicObject);
        static void RemoveBody_From_SimulateWorld(const Engine::Component::PhysicObject& physicObject);
        static void DeleteBody(const Engine::Component::PhysicObject& physicObject);
        static void Impulse(const Engine::Component::PhysicObject& physicObject, glm::vec3 force);
        static void AngularImpulse(const Engine::Component::PhysicObject& physicObject, glm::vec3 force);

    #pragma endregion 

    #pragma region Resize shape
    static JPH::BodyID ResizeBox(const Engine::Component::PhysicObject& physicObject, glm::vec3 newSize);

    static JPH::BodyID ResizeSphere(const Engine::Component::PhysicObject& physicObject, float newRadius);

    #pragma endregion

    #pragma region Setter
    static void Set_MotionType(const Engine::Component::PhysicObject& physicObject, JPH::EMotionType newMotionType);

    static void Set_Layer(const Engine::Component::PhysicObject& physicObject, JPH::ObjectLayer newLayers);

    static void Set_Gravity(const Engine::Component::PhysicObject& physicObject, float newValue);

    static void Set_Position(const Engine::Component::PhysicObject& physicObject, glm::vec3 position);

    static void Set_Rotation(const Engine::Component::PhysicObject& physicObject, glm::vec3 rotation);

    static void Set_Restitution(const Engine::Component::PhysicObject& physicObject, float newValue);

    static void Set_Friction(const Engine::Component::PhysicObject& physicObject, float newValue);

    static void Set_LinearDamping(const Engine::Component::PhysicObject& physicObject, float newValue);

    static void Set_AngularDamping(const Engine::Component::PhysicObject& physicObject, float newValue);

    #pragma endregion

    #pragma region Getter

    static glm::vec3 Get_Position(const Engine::Component::PhysicObject& physicObject);

    static glm::vec3 Get_CenterOfMass(const Engine::Component::PhysicObject& physicObject);

    static glm::vec3 Get_EulerRotation(const Engine::Component::PhysicObject& physicObject);

    static JPH::Quat Get_QuatRotation(const Engine::Component::PhysicObject& physicObject);

    static JPH::EMotionType Get_MotionType(const Engine::Component::PhysicObject& physicObject);

    static float Get_LinearVelocity(const Engine::Component::PhysicObject& physicObject);

    #pragma endregion
    static void DrawShape(const Engine::Component::PhysicObject& physicObject, JPH::DebugRenderer* debugRenderer, JPH::RMat44Arg center, JPH::ColorArg debugColor);

    private:
        inline static JPH::PhysicsSystem* physics_system = nullptr;
        inline static JPH::BodyInterface* body_interface = nullptr;
    };
}