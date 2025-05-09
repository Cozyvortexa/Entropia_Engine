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
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class RenderModule : public Module {
public:
	RenderModule();

	static RenderModule* GetInstance();

	void DrawTriangle();
	void DrawRectangle();


	void Init() override;
	void Render() override;
	void Shutdown() override;


private:
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	GLFWwindow* window = nullptr;
	Camera* mainCamera = nullptr;

	static RenderModule* instance;

	std::vector<unsigned int> shaderListe;

	glm::mat4 model = glm::mat4(1.0f);

};