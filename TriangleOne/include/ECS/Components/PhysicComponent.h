#pragma once

#include "Utilities/Observer.h"
#include "Physics/PhysicsHelper.h"

#include "ECS/Components/ComponentBase.h"

namespace Engine::Component {
	struct PhysicObject : public Component {
	public:
		~PhysicObject() {
			if (bodyID.IsInvalid()) {
				Engine::Physics::PhysicsHelper::DeleteBody(bodyID);
			}
		}

		PhysicObject(PhysicObject&& other) noexcept {
			motionType.Set(other.motionType.Get());
			gravity.Set(other.gravity.Get());
			gravityForce.Set(other.gravityForce.Get());
			bodyID = other.bodyID;
			offset = other.offset;

			SetupConnections();
		}
		PhysicObject& operator=(PhysicObject&& other) noexcept {
			if (this != &other) {
				//Clear current connection
				motionType_Connection = ScopedConnection();
				gravity_Connection = ScopedConnection();
				gravityForce_Connection = ScopedConnection();

				//Transfers the master data
				bodyID = other.bodyID;
				offset = other.offset;

				//Synchronises the values of the observers
				motionType.Set(other.motionType.Get());
				gravity.Set(other.gravity.Get());
				gravityForce.Set(other.gravityForce.Get());

				SetupConnections();
			}
			return *this;
		}
		PhysicObject(const PhysicObject&) = delete;
		PhysicObject& operator=(const PhysicObject&) = delete;


		inline JPH::BodyID& GetBodyID() { return bodyID; }


		glm::vec3 offset = glm::vec3(0);
		Observer<bool> gravity;
		Observer<float> gravityForce;
		Observer<JPH::EMotionType> motionType;
	private:
		void SetupConnections() {
			motionType_Connection = motionType.Subscribe([this](const JPH::EMotionType& value) {
				Engine::Physics::PhysicsHelper::SetMotionType(bodyID, value);
				});
			gravity_Connection = gravity.Subscribe([this](const bool& value) {
				Engine::Physics::PhysicsHelper::SetGravity(bodyID, value * gravityForce.Get());
				});
			gravityForce_Connection = gravityForce.Subscribe([this](const float& value) {
				Engine::Physics::PhysicsHelper::SetGravity(bodyID, value * gravity.Get());
				});
		}
		ScopedConnection motionType_Connection;
		ScopedConnection gravity_Connection;
		ScopedConnection gravityForce_Connection;
	protected:
		PhysicObject() {
			motionType.Set(JPH::EMotionType::Static);
			gravity.Set(false);
			gravityForce.Set(1.0f);
		};
		PhysicObject(JPH::EMotionType setMotionType) {
			motionType.Set(setMotionType);
			gravity.Set(false);
			gravityForce.Set(1.0f);
		}
		PhysicObject(JPH::EMotionType setMotionType, bool setGravity) {
			motionType.Set(setMotionType);
			gravity.Set(setGravity);
			gravityForce.Set(1.0f);
		}
		PhysicObject(JPH::EMotionType setMotionType, bool setGravity, float setGravityForce) {
			motionType.Set(setMotionType);
			gravity.Set(setGravity);
			gravityForce.Set(setGravityForce);
		}

		JPH::BodyID bodyID;
	};


	struct BoxCollider : public PhysicObject {
		BoxCollider() = delete;
		BoxCollider(glm::vec3 position) {
			boxSize.Set(glm::vec3(1.0));
			motionType.Set(JPH::EMotionType::Dynamic);
			bodyID = Engine::Physics::PhysicsHelper::CreateBox(position, boxSize.Get(), JPH::Quat::sIdentity(), motionType.Get());

			//Physics::PhysicsHelper::SetGravity(bodyID, gravity.Get());
			Engine::Physics::PhysicsHelper::AddBody_To_SimulateWorld(bodyID);
		}
		BoxCollider(glm::vec3 position, glm::vec3 size) : PhysicObject(JPH::EMotionType::Dynamic) {
			boxSize.Set(size);
			bodyID = Engine::Physics::PhysicsHelper::CreateBox(position, boxSize.Get(), JPH::Quat::sIdentity(), motionType.Get());

			//Physics::PhysicsHelper::SetGravity(bodyID, gravity.Get());
			Engine::Physics::PhysicsHelper::AddBody_To_SimulateWorld(bodyID);
		}
		BoxCollider(glm::vec3 position, glm::vec3 size, JPH::EMotionType setMotionType, bool setGravity, float setGravityForce ) : PhysicObject(setMotionType, setGravity, setGravityForce) {
			boxSize.Set(size);
			motionType.Set(JPH::EMotionType::Dynamic);
			bodyID = Engine::Physics::PhysicsHelper::CreateBox(position, boxSize.Get(), JPH::Quat::sIdentity(), motionType.Get());

			//Physics::PhysicsHelper::SetGravity(bodyID, gravity.Get());
			Engine::Physics::PhysicsHelper::AddBody_To_SimulateWorld(bodyID);
		}

		~BoxCollider() = default;

		BoxCollider(BoxCollider&& other) noexcept : PhysicObject(std::move(other)) {
			boxSize.Set(other.boxSize.Get());

			SetupConnections();
		}
		BoxCollider& operator=(BoxCollider&& other) noexcept { 
			if (this != &other) {
				PhysicObject::operator=(std::move(other));

				//Clear current connection
				boxSize_Connection = ScopedConnection();

				//Transfers the master dataD;
				offset = std::move(other.offset);

				//Synchronises the values of the observers
				boxSize.Set(other.boxSize.Get());

				SetupConnections();
			}
			return *this;
		}
		BoxCollider(const BoxCollider&) = delete;
		BoxCollider& operator=(const BoxCollider&) = delete;


		Observer<glm::vec3> boxSize;

		template<typename F>
		void Reflect(F&& f)
		{
			f("Gravity", gravity);
			f("GravityForce", gravityForce);
			f("boxSize:", boxSize);
			f("Offset", offset);
			f("Motion Type:", motionType);
		}
	private:
		void SetupConnections() {
			boxSize_Connection = boxSize.Subscribe([this](const glm::vec3& vec) {
				bodyID = Engine::Physics::PhysicsHelper::ResizeBox(bodyID, vec);
				});
		}

		ScopedConnection boxSize_Connection;
	};
}