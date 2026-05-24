#include "Systems/InputSystem.h"

namespace Resource = Engine::Resource;
namespace Component = Engine::Component;

void ScrollCallback(GLFWwindow* window, double xoffset, double ypos) {
	World* world = static_cast<World*>(glfwGetWindowUserPointer(window));  // Recupere l'instance du world
	Entity entityCam = world->get_ressource<Resource::ActiveCamera>()->cameraID;
	Component::CameraComponent* mainCamera = world->get_component<Component::CameraComponent>(entityCam);

	if (mainCamera != nullptr) {
		mainCamera->zoom -= (float)mainCamera->yoffset;
		if (mainCamera->zoom < 1.0f)
			mainCamera->zoom = 1.0f;
		if (mainCamera->zoom > 45.0f)
			mainCamera->zoom = 45.0f;
	}
	else {
		std::cout << "Main camera null dans Camera.cpp (ScrollCallBack)" << std::endl;
	}
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	World* world = static_cast<World*>(glfwGetWindowUserPointer(window));  // Recupere l'instance du world
	Entity entityCam = world->get_ressource<Resource::ActiveCamera>()->cameraID;
	//Entity entityCam = world->get_ressource<Resource::ActiveCamera>()->cameraID;
	Component::CameraComponent* mainCamera = world->get_component<Component::CameraComponent>(entityCam);
	Resource::InputResource* inputResource = world->get_ressource<Resource::InputResource>();

	if (!inputResource->mouseInputEnable) return;  // Cancel input

	if (mainCamera != nullptr) {
		if (mainCamera->firstMouse)
		{
			mainCamera->lastX = xpos;
			mainCamera->lastY = ypos;
			mainCamera->firstMouse = false;
		}

		float xoffset = xpos - mainCamera->lastX;
		float yoffset = mainCamera->lastY - ypos;

		mainCamera->lastX = xpos;
		mainCamera->lastY = ypos;

		const float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		mainCamera->yaw += xoffset;
		mainCamera->pitch += yoffset;

		mainCamera->yoffset = yoffset;
		// Blocage de la hauteur pour eviter les curse mouvement
		if (mainCamera->pitch > 89.0f)
			mainCamera->pitch = 89.0f;
		if (mainCamera->pitch < -89.0f)
			mainCamera->pitch = -89.0f;

		//Calcul de la nouvelle direction
		glm::vec3 direction;
		direction.x = cos(glm::radians(mainCamera->yaw)) * cos(glm::radians(mainCamera->pitch));
		direction.y = sin(glm::radians(mainCamera->pitch));
		direction.z = sin(glm::radians(mainCamera->yaw)) * cos(glm::radians(mainCamera->pitch));
		mainCamera->cameraFront = glm::normalize(direction);
	}
	else {
		std::cout << "No main camera in Camera.cpp (MouseCallback)" << std::endl;
	}
}


void Engine::Systems::InputSystem::Init(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	GLFWwindow* window = resourceBuffer->windowResource->window;

	glfwSetCursorPosCallback(window, MouseCallback);  // Pour permetre le mouvement de la cam
	glfwSetScrollCallback(window, ScrollCallback);
}

void Engine::Systems::InputSystem::Update(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	// A déplacer dans un input manager
	Entity entityCam = world.get_ressource<Resource::ActiveCamera>()->cameraID;
	Component::CameraComponent* mainCamera = world.get_component<Component::CameraComponent>(entityCam);
	Component::Transform* transformMainCamera = world.get_component<Component::Transform>(entityCam);
	Resource::InputResource* inputData = resourceBuffer->inputResource;
	ProcessInput(resourceBuffer->windowResource->window, mainCamera, transformMainCamera, resourceBuffer->timeResource->deltaTime, inputData);
	//
}


void Engine::Systems::InputSystem::ProcessInput(GLFWwindow* window, Component::CameraComponent* mainCamera, Component::Transform* transformMainCamera, float deltaTime, Resource::InputResource* inputData)
{
	const float speed = mainCamera->cameraSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		transformMainCamera->position += speed * mainCamera->cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		transformMainCamera->position -= speed * mainCamera->cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		transformMainCamera->position -= glm::normalize(glm::cross(mainCamera->cameraFront, mainCamera->cameraUp)) * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		transformMainCamera->position += glm::normalize(glm::cross(mainCamera->cameraFront, mainCamera->cameraUp)) * speed;

	//Mouvement supp
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		transformMainCamera->position += glm::vec3(0.0f, -1.0f, 0.0f) * speed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		transformMainCamera->position += glm::vec3(0.0f, 1.0f, 0.0f) * speed;

	//Mouse
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	if (state == GLFW_PRESS) {
		inputData->mouseInputEnable = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	}
	else {
		inputData->mouseInputEnable = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (state == GLFW_RELEASE) {
		mainCamera->firstMouse = true;
	}

}

void Engine::Systems::InputSystem::Shutdown(World& world) {
	std::cout << "InputSystem shutting down" << std::endl;
}