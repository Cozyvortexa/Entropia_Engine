#include "Systems/windowSystem.h"

WindowSystem::WindowSystem() {}

void GLAPIENTRY DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam) {

	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;  // Ignore warning

	std::cerr << "--- OPENGL DEBUG MESSAGE ---" << std::endl;
	std::cerr << "Source: " << source << " | Type: " << type << std::endl;
	std::cerr << "ID: " << id << " | Severity: " << severity << std::endl;
	std::cerr << "Message: " << message << std::endl;
	std::cerr << "----------------------------" << std::endl;

	if (severity == GL_DEBUG_SEVERITY_HIGH) {
		std::cerr << "CRITICAL ERROR DETECTED!" << std::endl;
		__debugbreak(); // abort() like
	}
}
void GLAPIENTRY Debug_Critical_MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam) {
	if (severity == GL_DEBUG_SEVERITY_HIGH) {
		std::cerr << "--- OPENGL DEBUG MESSAGE ---" << std::endl;
		std::cerr << "Source: " << source << " | Type: " << type << std::endl;
		std::cerr << "ID: " << id << " | Severity: " << severity << std::endl;
		std::cerr << "Message: " << message << std::endl;
		std::cerr << "----------------------------" << std::endl;
		std::cerr << "CRITICAL ERROR DETECTED!" << std::endl;
		__debugbreak(); // abort() like
	}
}

void WindowSystem::Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

void WindowSystem::ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void WindowSystem::Init(World& world, const ResourceBuffer* resourceBuffer) {
	WindowResource* windowData = resourceBuffer->windowResource;
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
#ifndef NDEBUG

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(Debug_Critical_MessageCallback, 0);

#endif

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

bool WindowSystem::ShouldClose(World& world) {
	WindowResource* ressource = world.get_ressource<WindowResource>();  // WARNING
	if (!glfwWindowShouldClose(ressource->window)) {
		return true;
	}
	return false;
}


void WindowSystem::Update(World& world, const ResourceBuffer* resourceBuffer) {
	WindowResource* windowData = resourceBuffer->windowResource;
	ProcessInput(windowData->window);  // gere les inputs 
}


void WindowSystem::Shutdown(World& world) {
	WindowResource* Resource = world.get_ressource<WindowResource>();

	std::cout << "Window shutting down" << std::endl;
	if (Resource->window != nullptr) {
		glfwDestroyWindow(Resource->window);
		Resource->window = nullptr;
	}
	glfwTerminate();
}

