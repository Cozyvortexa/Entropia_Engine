//#include "Camera.h"
//
//
//void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
//	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));  // Recupere l'instance de camera actuellement lie
//	if (camera) {
//		camera->ProcessCameraMouseInput(xpos, ypos);
//	}
//	else {
//		std::cout << "Camera non lie dans Camera.cpp" << std::endl;
//	}
//}
//
//void ScrollCallback(GLFWwindow* window, double xoffset, double ypos) {
//	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));  // Recupere l'instance de camera actuellement lie
//	if (camera) {
//		camera->ProcessMouseScroll(xoffset, ypos);
//	}
//	else {
//		std::cout << "Camera non lie dans Camera.cpp (ScrollCallBack)" << std::endl;
//	}
//}
//
//
//Camera::Camera(glm::vec3 Pos) {
//	cameraPos = Pos;
//	Window* windowClass = Window::GetInstance();
//
//	//window = windowClass->GetWindow();
//	glfwSetWindowUserPointer(window, this);  // Lie la cam a la fenetre
//	glfwSetCursorPosCallback(window, MouseCallback);  // Pour permetre le mouvement de la cam
//	glfwSetScrollCallback(window, ScrollCallback);
//}
//
//void Camera::ProcessCameraMouseInput(double xpos, double ypos) {
//
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos;
//
//	lastX = xpos;
//	lastY = ypos;
//
//	const float sensitivity = 0.1f;
//	xoffset *= sensitivity;
//	yoffset *= sensitivity;
//
//	yaw += xoffset;
//	pitch += yoffset;
//
//	// Blocage de la hauteur pour eviter les curse mouvement
//	if (pitch > 89.0f)
//		pitch = 89.0f;
//	if (pitch < -89.0f)
//		pitch = -89.0f;
//
//	//Calcul de la nouvelle direction
//	glm::vec3 direction;
//	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//	direction.y = sin(glm::radians(pitch));
//	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//	cameraFront = glm::normalize(direction);
//}
//
//glm::mat4 Camera::GetViewMatrix() {
//	glm::mat4 view;
//	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
//	return view;
//}
//
//void Camera::ProcessInput(GLFWwindow* window)
//{
//	float deltaTime = Time::GetDeltaTime();
//	const float cameraSpeed = 9.0f * deltaTime;
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		cameraPos += cameraSpeed * cameraFront;
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		cameraPos -= cameraSpeed * cameraFront;
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//
//	//Mouvement supp
//	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//		cameraPos += glm::vec3(0.0f, -1.0f, 0.0f) * cameraSpeed;
//	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//		cameraPos += glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;
//}
//
//
//void Camera::ProcessMouseScroll(double xoffset, float yoffset){
//
//	zoom -= (float)yoffset;
//	if (zoom < 1.0f)
//		zoom = 1.0f;
//	if (zoom > 45.0f)
//		zoom = 45.0f;
//}