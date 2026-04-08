#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "ECS/System.h"
#include "ECS/Components/Component.h"
#include "ECS/World.h"

#include <iostream>

class InputSystem : public System {
public:
	virtual ~InputSystem() = default;
	void Init(World& world, const ResourceBuffer* resourceBuffer) override;
	void Update(World& world, const ResourceBuffer* ressourceBuffer) override;
	void Shutdown(World& world) override;

	void ProcessInput(GLFWwindow* window, CameraComponent* mainCamera, Transform* transformMainCamera, float deltaTime, InputResource* inputData);
};