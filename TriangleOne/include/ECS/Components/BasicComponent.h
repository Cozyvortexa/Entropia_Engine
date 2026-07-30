#pragma once
#include "ECS/Components/ComponentBase.h"

class Mesh;
namespace Engine::Component {
	struct Transform : public Component
	{
		Transform() {};
		Transform(glm::vec3 position) { this->position = position; }
		Transform(glm::vec3 position, glm::vec3 rotation) { this->position = position; this->rotation = rotation; }
		Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) { this->position = position; this->rotation = rotation; this->scale = scale; }
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);


		template<typename F>
		void Reflect(F&& f)
		{
			f("Position:", position);
			f("Rotation:", rotation);
			f("Scale:", scale);
		}

		glm::mat4 GetTransformModel() const {
			glm::mat4 model(1.0f);


			model = glm::translate(model, position);

			model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
			model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
			model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

			model = glm::scale(model, glm::vec3(scale));

			return model;
		}
	};

	struct CameraComponent : public Component {
		CameraComponent(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT) {
			this->SCR_WIDTH = SCR_WIDTH;
			this->SCR_HEIGHT = SCR_HEIGHT;
			lastX = SCR_WIDTH / 2.0f;
			lastY = SCR_HEIGHT / 2.0f;
		}
		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);

		float cameraSpeed = 9.0f;

		float yoffset = 0.0f;

		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		float nearPlane = 0.1f;
		float farPlane = 200.0f;

		float zoom = 45.0f;  // Default zoom value 
		//Rotation
		float yaw = -90.0f;
		float pitch = 0.0f;

		bool firstMouse = true;

		unsigned int SCR_WIDTH = 0;
		unsigned int SCR_HEIGHT = 0;

		float lastX = 1.0f;
		float lastY = 1.0f;

		glm::mat4 viewMatrice = glm::mat4(1.0f);
	};

	struct SceneTag : public Component {
		SceneTag() = default;
		SceneTag(std::string name) { this->name = name; }
		uint16_t scene_id = 0;  // Zero correspond to the main scene 
		std::string name = "Default";
	};


	struct MeshHandle : public Component {
		MeshHandle() {};
		MeshHandle(std::shared_ptr<Mesh> meshPtr) { this->meshPtr = meshPtr; }
		MeshHandle(std::shared_ptr<Mesh> meshPtr, bool castShadow, bool haveToBeDraw = true) {
			this->meshPtr = meshPtr; 
			this->castShadow = castShadow; 
			this->haveToBeDraw = haveToBeDraw; 
		}

		std::shared_ptr<Mesh> meshPtr;
		bool castShadow = true;
		bool haveToBeDraw = true;

		template<typename F>
		void Reflect(F&& f)
		{
			f("HaveToBeDraw", haveToBeDraw);
			f("CastShadow", castShadow);
			f("Mesh", meshPtr);
		}
	};

	struct MaterialHandle : Component {
		MaterialHandle() {};
		MaterialHandle(uint32_t index) { this->index = index; }
		uint32_t index = 0;
	};

}