#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "Module.h"

#include <fstream>
#include <sstream>

#include <iostream>
#include <vector>

#include "Shader.h"
#include "window.h"
#include "Time.hpp"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class RenderModule : public Module {
public:
	RenderModule();

	static RenderModule* GetInstance();

	void DrawTriangle();
	void DrawRectangle();
	glm::mat4 Camera();

	void ProcessInput(GLFWwindow* window);

	void Init() override;
	void Render() override;
	void Shutdown() override;

	void MouseCallback(GLFWwindow* window, double xpos, double ypos);

private:
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	GLFWwindow* window = nullptr;

	static RenderModule* instance;

	std::vector<unsigned int> shaderListe;

	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);



	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	inline static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);

	//Rotation
	inline static float yaw = -90.0f;
	inline static float pitch = 0.0f;

	inline static float lastX = 400, lastY = 300;
	inline static bool firstMouse = true;
};