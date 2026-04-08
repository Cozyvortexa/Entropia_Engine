#include "Systems/CameraSystem.h"


void CameraSystem::Init(World& world, const ResourceBuffer* resourceBuffer) {
	GLFWwindow* window = resourceBuffer->windowResource->window;
	glfwSetWindowUserPointer(window, &world);  // Bind du world a la fenetre
}

void CameraSystem::Update(World& world, const ResourceBuffer* resourceBuffer) {
	View view = world.view<CameraComponent, Transform>();
	view.each([&](int entity, CameraComponent& currentCamera, Transform& transform) {
		currentCamera.viewMatrice = glm::lookAt(transform.position, transform.position + currentCamera.cameraFront, currentCamera.cameraUp);
	});

}