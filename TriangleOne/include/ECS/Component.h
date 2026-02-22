#pragma once

struct Component {};

struct Ressource {};

struct WindowRessource : public Ressource {
	inline static int WIDHT = 800;
	inline static int HEIGHT = 600;

	int sample = 4;

	GLFWwindow* window = nullptr;
};

struct Position : public Component {
	int x;
	int y;
};