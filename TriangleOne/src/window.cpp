#include "window.h"

Window* Window::instance = nullptr;

Window::Window() {
	instance = this;
}

Window* Window::GetInstance() {
	return instance;
}

void Window::Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

void Window::ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void Window::Init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//MSAA
	glfwWindowHint(GLFW_SAMPLES, sample);

	window = glfwCreateWindow(WIDHT, HEIGHT, "Entropia Engine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		abort();
	}

	glfwMakeContextCurrent(window);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		abort();
	}
	glfwSetFramebufferSizeCallback(window, Framebuffer_size_callback); // Pour adapter le viewport si la fenetre est resize pendant le court du programme 
	glViewport(0, 0, WIDHT, HEIGHT);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

bool Window::ShouldClose() {
	if (!glfwWindowShouldClose(window)) {
		return true;
	}
	return false;
}


void Window::Update(World& world) {
	ProcessInput(window);  // gere les inputs 
}


void Window::Shutdown() {
	std::cout << "Window shutting down" << std::endl;
	if (window != nullptr) {
		glfwDestroyWindow(window);
		window = nullptr;
	}
	glfwTerminate();
}