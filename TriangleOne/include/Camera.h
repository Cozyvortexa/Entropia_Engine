#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include "Time.hpp"

class Camera {
public:
	Camera(glm::vec3 Pos);

	glm::mat4 GetViewMatrix();

	void ProcessCameraMouseInput(double xpos, double ypos);

	void ProcessInput(GLFWwindow* window);

	void ProcessMouseScroll(double xoffset, float yoffset);

	float GetZoom() { return zoom; };

	glm::vec3 GetPos() { return cameraPos; }

private:
	GLFWwindow* window = nullptr;

	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);


	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


	float zoom = 45.0f;  // valeur de zoom par default 
	//Rotation
	float yaw = -90.0f;
	float pitch = 0.0f;

	bool firstMouse = true;

	const unsigned int SCR_WIDTH = Window::GetWidth();
	const unsigned int SCR_HEIGHT = Window::GetHeight();

	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;
};