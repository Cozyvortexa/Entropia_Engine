#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

struct Component {
	virtual ~Component() = default;
};

struct Ressource {};

struct WindowRessource : public Ressource {
	inline static int WIDHT = 800;
	inline static int HEIGHT = 600;

	int sample = 4;

	GLFWwindow* window = nullptr;
};

struct TimeRessource : public Ressource {
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

};

struct Position : public Component {
	int x;
	int y;
};