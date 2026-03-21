#include "Systems/CameraSystem.h"


void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	World* world = static_cast<World*>(glfwGetWindowUserPointer(window));  // Recupere l'instance du world
	Entity entityCam = world->get_ressource<ActiveCamera>()->cameraID;
	CameraComponent* mainCamera = world->get_component<CameraComponent>(entityCam);

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
		std::cout << "Main camera null dans Camera.cpp (MouseCallback)" << std::endl;
	}
}

void ScrollCallback(GLFWwindow* window, double xoffset, double ypos) {
	World* world = static_cast<World*>(glfwGetWindowUserPointer(window));  // Recupere l'instance du world
	Entity entityCam = world->get_ressource<ActiveCamera>()->cameraID;
	CameraComponent* mainCamera = world->get_component<CameraComponent>(entityCam);

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


void CameraSystem::Init(World& world, const ResourceBuffer* resourceBuffer) {
	GLFWwindow* window = resourceBuffer->windowResource->window;

	glfwSetWindowUserPointer(window, &world);  // Bind du world a la fenetre
	glfwSetCursorPosCallback(window, MouseCallback);  // Pour permetre le mouvement de la cam
	glfwSetScrollCallback(window, ScrollCallback);
}

void CameraSystem::Update(World& world, const ResourceBuffer* resourceBuffer) {
	View view = world.view<CameraComponent, Transform>();
	view.each([&](int entity, CameraComponent& currentCamera, Transform& transform) {
		currentCamera.viewMatrice = glm::lookAt(transform.position, transform.position + currentCamera.cameraFront, currentCamera.cameraUp);
	});


	// A déplacer dans un input manager
	Entity entityCam = world.get_ressource<ActiveCamera>()->cameraID;
	CameraComponent* mainCamera = world.get_component<CameraComponent>(entityCam);
	Transform* transformMainCamera = world.get_component<Transform>(entityCam);
	ProcessInput(resourceBuffer->windowResource->window, mainCamera, transformMainCamera, resourceBuffer->timeResource->deltaTime);
	//
}

void CameraSystem::ProcessInput(GLFWwindow* window, CameraComponent* mainCamera, Transform* transformMainCamera, float deltaTime)	
{
	const float cameraSpeed = 9.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		transformMainCamera->position += cameraSpeed * mainCamera->cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		transformMainCamera->position -= cameraSpeed * mainCamera->cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		transformMainCamera->position -= glm::normalize(glm::cross(mainCamera->cameraFront, mainCamera->cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		transformMainCamera->position += glm::normalize(glm::cross(mainCamera->cameraFront, mainCamera->cameraUp)) * cameraSpeed;

	//Mouvement supp
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		transformMainCamera->position += glm::vec3(0.0f, -1.0f, 0.0f) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		transformMainCamera->position += glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;
}


