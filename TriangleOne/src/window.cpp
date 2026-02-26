#include "window.h"

Window::Window() {}


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

void Window::Init(World& world) {
	WindowResource* windowData = world.get_ressource<WindowResource>();
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//MSAA
	glfwWindowHint(GLFW_SAMPLES, windowData->sample);

	windowData->window = glfwCreateWindow(windowData->WIDHT, windowData->HEIGHT, "Entropia Engine", NULL, NULL);
	if (windowData->window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		abort();
	}

	glfwMakeContextCurrent(windowData->window);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		abort();
	}
	glfwSetFramebufferSizeCallback(windowData->window, Framebuffer_size_callback); // Pour adapter le viewport si la fenetre est resize pendant le court du programme 
	glViewport(0, 0, windowData->WIDHT, windowData->HEIGHT);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


	glfwSetInputMode(windowData->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Blending     //ya pas de blending mm avec cette ligne au cas ou 
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Face culling     //fonctionne bizzarement ( faudrait chek l'ordre de dessin des vertex)
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	//MSAA
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	//Correction gamma 
	//glEnable(GL_FRAMEBUFFER_SRGB);
}

bool Window::ShouldClose(World& world) {
	WindowResource* ressource = world.get_ressource<WindowResource>();  // WARNING
	if (!glfwWindowShouldClose(ressource->window)) {
		return true;
	}
	return false;
}


void Window::Update(World& world, const ResourceBuffer* resourceBuffer) {
	WindowResource* windowData = resourceBuffer->windowResource;
	ProcessInput(windowData->window);  // gere les inputs 
}


void Window::Shutdown(World& world) {
	WindowResource* Resource = world.get_ressource<WindowResource>();

	std::cout << "Window shutting down" << std::endl;
	if (Resource->window != nullptr) {
		glfwDestroyWindow(Resource->window);
		Resource->window = nullptr;
	}
	glfwTerminate();
}