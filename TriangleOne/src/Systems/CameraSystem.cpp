#include "Systems/CameraSystem.h"

namespace Resource = Engine::Resource;
namespace Component = Engine::Component;
namespace Systems = Engine::Systems;

void Systems::CameraSystem::Init(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	GLFWwindow* window = resourceBuffer->windowResource->window;
	glfwSetWindowUserPointer(window, &world);  // Bind du world a la fenetre
}

void Systems::CameraSystem::Update(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	View view = world.view<Component::CameraComponent, Component::Transform>();
	view.each([&](int entity, Component::CameraComponent& currentCamera, Component::Transform& transform) {
		currentCamera.viewMatrice = glm::lookAt(transform.position, transform.position + currentCamera.cameraFront, currentCamera.cameraUp);
	});

}