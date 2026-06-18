#pragma once

#include "Utilities/Observer.h"
#include "Physics/PhysicsHelper.h"

#include "ECS/Components/ComponentBase.h"

namespace Engine::Component {
	struct PhysicObject : public Component {
	public:
		~PhysicObject() {
			//objectIsValid is set to false if the data has been moved
			if (objectIsValid) {
				Engine::Physics::PhysicsHelper::DeleteBody(*this);
			}
		}

		PhysicObject(PhysicObject&& other) noexcept {
			motionType.Set(other.motionType.Get());
			gravity.Set(other.gravity.Get());
			gravityForce.Set(other.gravityForce.Get());
			bodyID = other.bodyID;
			offset = other.offset;
			displayShape = other.displayShape;

			//Invalid other
			other.objectIsValid = false;

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
				displayShape = other.displayShape;

				//Invalid other
				other.objectIsValid = false;

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

		bool displayShape = true;
		glm::vec3 offset = glm::vec3(0);
		Observer<bool> gravity;
		Observer<float> gravityForce;
		Observer<JPH::EMotionType> motionType;
	private:
		bool objectIsValid = true;
		void SetupConnections() {
			motionType_Connection = motionType.Subscribe([this](const JPH::EMotionType& value) {
				Engine::Physics::PhysicsHelper::Set_MotionType(*this, value);
				});
			gravity_Connection = gravity.Subscribe([this](const bool& value) {
				Engine::Physics::PhysicsHelper::Set_Gravity(*this, value * gravityForce.Get());
				});
			gravityForce_Connection = gravityForce.Subscribe([this](const float& value) {
				Engine::Physics::PhysicsHelper::Set_Gravity(*this, value * gravity.Get());
				});
		}

		ScopedConnection motionType_Connection;
		ScopedConnection gravity_Connection;
		ScopedConnection gravityForce_Connection;
	protected:
		PhysicObject() {
			SetDefaultValue();
		};
		PhysicObject(JPH::EMotionType setMotionType) {
			SetDefaultValue(setMotionType);
		}
		PhysicObject(JPH::EMotionType setMotionType, bool setGravity) {
			SetDefaultValue(setMotionType, setGravity);
		}
		PhysicObject(JPH::EMotionType setMotionType, bool setGravity, float setGravityForce) {
			SetDefaultValue(setMotionType, setGravity, setGravityForce);
		}

		inline void SetDefaultValue(JPH::EMotionType setMotionType = JPH::EMotionType::Static, bool setGravity = false, float setGravityForce = 1) {
			motionType.Set(setMotionType);
			gravity.Set(setGravity);
			gravityForce.Set(setGravityForce);
		}

		inline void Add_To_Simulation() {
			Engine::Physics::PhysicsHelper::Set_Gravity(*this, gravity.Get() * gravityForce.Get());
			Engine::Physics::PhysicsHelper::AddBody_To_SimulateWorld(*this);
		}

		friend Engine::Physics::PhysicsHelper;
		JPH::BodyID bodyID;
	};

	struct BoxCollider : public PhysicObject {
		BoxCollider() = delete;

		BoxCollider(glm::vec3 position) : PhysicObject(JPH::EMotionType::Dynamic) {
			boxSize.Set(glm::vec3(1.0));
			bodyID = Engine::Physics::PhysicsHelper::CreateBox(position, boxSize.Get(), JPH::Quat::sIdentity(), motionType.Get());

			Add_To_Simulation();
		}

		BoxCollider(glm::vec3 position, glm::vec3 size) : PhysicObject(JPH::EMotionType::Dynamic) {
			boxSize.Set(size);
			bodyID = Engine::Physics::PhysicsHelper::CreateBox(position, boxSize.Get(), JPH::Quat::sIdentity(), motionType.Get());

			Add_To_Simulation();
		}

		BoxCollider(glm::vec3 position, glm::vec3 size, JPH::EMotionType setMotionType, bool setGravity, float setGravityForce ) : PhysicObject(setMotionType, setGravity, setGravityForce) {
			boxSize.Set(size);
			bodyID = Engine::Physics::PhysicsHelper::CreateBox(position, boxSize.Get(), JPH::Quat::sIdentity(), motionType.Get());

			Add_To_Simulation();
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
			f("Gravity:", gravity);
			f("Display shape:", displayShape);
			f("GravityForce:", gravityForce);
			f("boxSize:", boxSize);
			f("Offset:", offset);
			f("Motion Type:", motionType);
		}
	private:
		void SetupConnections() {
			boxSize_Connection = boxSize.Subscribe([this](const glm::vec3& vec) {
				bodyID = Engine::Physics::PhysicsHelper::ResizeBox(*this, vec);
				Add_To_Simulation();
				});
		}

		ScopedConnection boxSize_Connection;
	};



	struct SphereCollider : public PhysicObject {
		SphereCollider() = delete;

		SphereCollider(glm::vec3 position) : PhysicObject(JPH::EMotionType::Dynamic) {
			radius.Set(1.0f);
			bodyID = Engine::Physics::PhysicsHelper::CreateSphere(position, radius.Get(), JPH::Quat::sIdentity(), motionType.Get());

			Add_To_Simulation();
		}

		SphereCollider(glm::vec3 position, float setRadius) : PhysicObject(JPH::EMotionType::Dynamic) {
			radius.Set(setRadius);
			bodyID = Engine::Physics::PhysicsHelper::CreateSphere(position, radius.Get(), JPH::Quat::sIdentity(), motionType.Get());

			Add_To_Simulation();
		}

		SphereCollider(glm::vec3 position, float setRadius, JPH::EMotionType setMotionType, bool setGravity, float setGravityForce) : PhysicObject(setMotionType, setGravity, setGravityForce) {
			radius.Set(setRadius);
			bodyID = Engine::Physics::PhysicsHelper::CreateSphere(position, radius.Get(), JPH::Quat::sIdentity(), motionType.Get());

			Add_To_Simulation();
		}


		~SphereCollider() = default;


		SphereCollider(SphereCollider&& other) noexcept : PhysicObject(std::move(other)) {
			radius.Set(other.radius.Get());

			SetupConnections();
		}
		SphereCollider& operator=(SphereCollider&& other) noexcept {
			if (this != &other) {
				PhysicObject::operator=(std::move(other));

				//Clear current connection
				radius_Connection = ScopedConnection();

				//Transfers the master dataD;
				offset = std::move(other.offset);

				//Synchronises the values of the observers
				radius.Set(other.radius.Get());

				SetupConnections();
			}
			return *this;
		}
		SphereCollider(const SphereCollider&) = delete;
		SphereCollider& operator=(const SphereCollider&) = delete;


		Observer<float> radius;

		template<typename F>
		void Reflect(F&& f)
		{
			f("Gravity:", gravity);
			f("Display shape:", displayShape);
			f("GravityForce:", gravityForce);
			f("Radius:", radius);
			f("Offset:", offset);
			f("Motion Type:", motionType);
		}
	private:
		void SetupConnections() {
			radius_Connection = radius.Subscribe([this](const float& value) {
				bodyID = Engine::Physics::PhysicsHelper::ResizeSphere(*this, value);
				Add_To_Simulation();
				});
		}

		ScopedConnection radius_Connection;
	};



	struct ConvexShape : public PhysicObject {
		ConvexShape() : PhysicObject(JPH::EMotionType::Dynamic) {}

		ConvexShape(glm::vec3 position, const std::vector<glm::vec3> verticesPos) : PhysicObject(JPH::EMotionType::Dynamic) {
			JPH::Array<JPH::Vec3> points;
			for (const glm::vec3& currentVertice : verticesPos) {
				points.push_back(Engine::Physics::PhysicsHelper::To_Vec3_JPH(currentVertice));
			}


			bodyID = Engine::Physics::PhysicsHelper::CreateConvexShape(points, position, JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Engine::Physics::Layers::MOVING);
			Add_To_Simulation();
		}
	};
}