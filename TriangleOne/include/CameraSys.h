#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include "Systemes/TimeSysteme/Time.hpp"

class CameraSys : public System {
public:
	void Init(World& world) override;

	void Update(World& world, const ResourceBuffer* resourceBuffer) override;

	void ProcessInput(GLFWwindow* window, CameraComponent* mainCamera, Transform* transformMainCamera, float deltaTime);


};