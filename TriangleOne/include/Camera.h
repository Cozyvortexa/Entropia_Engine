#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include "Systemes/TimeSysteme/Time.hpp"

class Camera {
public:
	Camera(glm::vec3 Pos);

	glm::mat4 GetViewMatrix();

	void ProcessCameraMouseInput(double xpos, double ypos);

	void ProcessInput(GLFWwindow* window);

	void ProcessMouseScroll(double xoffset, float yoffset);

private:
	GLFWwindow* window = nullptr;

};