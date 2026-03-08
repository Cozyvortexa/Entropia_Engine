#pragma once
#include "string.h"
#include "Render/Shader.h"

struct Material {
	Material(std::string name, const char* vertexPath, const char* fragmentPath) {
		shader = Shader(vertexPath, fragmentPath);
		this->name = name;
	};
	std::string name;
	Shader shader;
};