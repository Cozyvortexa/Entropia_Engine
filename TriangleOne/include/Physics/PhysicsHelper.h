#pragma once
#include "ECS/Components/PhysicsStruct.h"
#include <assert.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Engine::Physics {
    class PhysicsHelper {
    public:
        static void Init(JPH::BodyInterface& new_body_interface);
        //static JPH::BodyInterface& GetBodyInterface();

    #pragma region Converter
        static inline JPH::Vec3 To_Vec3_JPH(const glm::vec3 vec);
        static inline JPH::RVec3 To_Rvec3_JPH(const glm::vec3 vec);
        static inline glm::vec3 To_Vec3_GLM(const JPH::Vec3& v);

        static inline glm::vec3 JoltQuatToEuler(const JPH::Quat& q);
        static inline glm::vec3 JoltQuatToEulerDegrees(const JPH::Quat& q);

    #pragma endregion

    #pragma region Create Shape
        static JPH::BodyID CreateBody_With_Param(JPH::BoxShapeSettings& shape_settings, glm::vec3 position);
        static JPH::BodyID CreateBody_With_Param(JPH::BoxShapeSettings& shape_settings, glm::vec3 position, JPH::Quat quaternion);
        static JPH::BodyID CreateBody_With_Param(JPH::BoxShapeSettings& shape_settings, glm::vec3 position, JPH::Quat quaternion, JPH::EMotionType motionType);
        static JPH::BodyID CreateBody_With_Param(JPH::BoxShapeSettings& shape_settings, glm::vec3 position, JPH::Quat quaternion, JPH::EMotionType motionType, JPH::ObjectLayer layers);

        static JPH::BodyID CreateBox(const glm::vec3 position);
        static JPH::BodyID CreateBox(const glm::vec3 position, glm::vec3 size);
        static JPH::BodyID CreateBox(const glm::vec3 position, glm::vec3 size, JPH::Quat quaternion);
        static JPH::BodyID CreateBox(const glm::vec3 position, glm::vec3 size, JPH::Quat quaternion, JPH::EMotionType motionType);

    #pragma endregion

    #pragma region Simulated World Interaction
        static void AddBody_To_SimulateWorld(const JPH::BodyID& bodyID);
        static void RemoveBody_From_SimulateWorld(const JPH::BodyID& bodyID);
        static void DeleteBody(const JPH::BodyID& bodyID);

    #pragma endregion 

    #pragma region Resize shape
    static JPH::BodyID ResizeBox(const JPH::BodyID& bodyID, glm::vec3 newSize);

    #pragma endregion

    #pragma region Setter
    static void SetMotionType(const JPH::BodyID& bodyID, JPH::EMotionType newMotionType);

    static void SetLayer(const JPH::BodyID& bodyID, JPH::ObjectLayer newLayers);

    static void SetGravity(const JPH::BodyID& bodyID, bool newValue);

    #pragma endregion

    #pragma region Getter

    static glm::vec3 GetCenterOfMass(const JPH::BodyID& bodyID);
    static glm::vec3 GetRotation(const JPH::BodyID& bodyID);

    #pragma endregion

    private:
        inline static JPH::BodyInterface* body_interface = nullptr;
    };
}