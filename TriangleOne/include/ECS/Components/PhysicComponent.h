#pragma once

#include "Utilities/Observer.h"
#include "Physics/PhysicsHelper.h"

#include "ECS/Components/ComponentBase.h"

namespace Engine::Component {
	struct PhysicObject : public Component {
	public:
		inline JPH::BodyID& GetBodyID() { return bodyID; }
	protected:
		JPH::BodyID bodyID;
	};

	struct BoxCollider : public PhysicObject {
		BoxCollider() = delete;
		BoxCollider(glm::vec3 position) {
			boxSize.Set(glm::vec3(1.0));
			motionType.Set(JPH::EMotionType::Dynamic);
			//gravity.Set(false);
			bodyID = Engine::Physics::PhysicsHelper::CreateBox(position, boxSize.Get(), JPH::Quat::sIdentity(), motionType.Get());

			//Physics::PhysicsHelper::SetGravity(bodyID, gravity.Get());
			Engine::Physics::PhysicsHelper::AddBody_To_SimulateWorld(bodyID);
		}
		BoxCollider(glm::vec3 position, glm::vec3 size) {
			boxSize.Set(size);
			motionType.Set(JPH::EMotionType::Dynamic);
			//gravity.Set(false);
			bodyID = Engine::Physics::PhysicsHelper::CreateBox(position, boxSize.Get(), JPH::Quat::sIdentity(), motionType.Get());

			//Physics::PhysicsHelper::SetGravity(bodyID, gravity.Get());
			Engine::Physics::PhysicsHelper::AddBody_To_SimulateWorld(bodyID);
		}

		~BoxCollider() {
			if (bodyID.IsInvalid()) {
				Engine::Physics::PhysicsHelper::DeleteBody(bodyID);
			}
		}

		BoxCollider(BoxCollider&& other) noexcept {
			boxSize.Set(other.boxSize.Get());
			motionType.Set(other.motionType.Get());
			gravity.Set(other.gravity.Get());
			bodyID = other.bodyID;

			SetupConnections();
		}
		BoxCollider& operator=(BoxCollider&& other) noexcept {
			if (this != &other) {
				//Clear current connection
				boxSize_Connection = ScopedConnection();
				motionType_Connection = ScopedConnection();
				gravity_Connection = ScopedConnection();

				//Transfers the master data
				bodyID = other.bodyID;
				offset = std::move(other.offset);

				//Synchronises the values of the observers
				boxSize.Set(other.boxSize.Get());
				motionType.Set(other.motionType.Get());
				gravity.Set(other.gravity.Get());

				SetupConnections();
			}
			return *this;
		}
		BoxCollider(const BoxCollider&) = delete;
		BoxCollider& operator=(const BoxCollider&) = delete;


		glm::vec3 offset = glm::vec3(0);
		Observer<glm::vec3> boxSize;

		Observer<bool> gravity;

		Observer<JPH::EMotionType> motionType;

		template<typename F>
		void Reflect(F&& f)
		{
			f("Gravity", gravity);
			f("boxSize:", boxSize);
			f("Offset", offset);
			f("Motion Type:", motionType);
		}
	private:
		void SetupConnections() {
			boxSize_Connection = boxSize.Subscribe([this](const glm::vec3& vec) {
				bodyID = Engine::Physics::PhysicsHelper::ResizeBox(bodyID, vec);
				});
			motionType_Connection = motionType.Subscribe([this](const JPH::EMotionType& value) {
				Engine::Physics::PhysicsHelper::SetMotionType(bodyID, value);
				});
			gravity_Connection = gravity.Subscribe([this](const bool& value) {
				Engine::Physics::PhysicsHelper::SetGravity(bodyID, value);
				});
		}

		ScopedConnection boxSize_Connection;
		ScopedConnection motionType_Connection;
		ScopedConnection gravity_Connection;
	};
}