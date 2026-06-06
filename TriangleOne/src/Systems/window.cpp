#include "Systems/windowSystem.h"

#include "Utilities/ImGui/imgui.h"
#include "Utilities/ImGui/imgui_internal.h"
#include "Utilities/ImGui/backends/imgui_impl_glfw.h"
#include "Utilities/ImGui/backends/imgui_impl_opengl3.h"

namespace Resource = Engine::Resource;
namespace Systems = Engine::Systems;

Systems::WindowSystem::WindowSystem() {}

#pragma region CallBack func

#pragma region OpenGl_Error
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

#pragma endregion


void Systems::WindowSystem::Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//glViewport(0, 0, width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	World* world = static_cast<World*>(glfwGetWindowUserPointer(window));
	Resource::RenderResource* renderResource = world->get_ressource<Resource::RenderResource>();
	Resource::InterfaceRessource* interfaceResource = world->get_ressource<Resource::InterfaceRessource>();
	Resource::WindowResource* windowData = world->get_ressource<Resource::WindowResource>();
	windowData->HEIGHT = height;
	windowData->WIDTH = width;


	//FallBack
	if (!interfaceResource->OnEditorView) {
		RenderSystem::ResizeFrameBufferText(renderResource);
	}
}

void Systems::WindowSystem::Window_iconify_callback(GLFWwindow* window, int iconified) {
	World* world = static_cast<World*>(glfwGetWindowUserPointer(window));
	Resource::WindowResource* windowData = world->get_ressource<Resource::WindowResource>();

	if (iconified) {
		windowData->isIconified = true;
	}
	else {
		windowData->isIconified = false;
	}
}

#pragma endregion

void Systems::WindowSystem::ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void Systems::WindowSystem::Init(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::WindowResource* windowData = resourceBuffer->windowResource;
	Resource::RenderResource* renderData = resourceBuffer->renderResource;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//MSAA
	glfwWindowHint(GLFW_SAMPLES, 0);

	windowData->window = glfwCreateWindow(windowData->WIDTH, windowData->HEIGHT, "Entropia Engine", NULL, NULL);
	if (windowData->window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		abort();
	}

	glfwMakeContextCurrent(windowData->window);
	glfwSwapInterval(1);  //Disable VSYNC

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
	//world.renderer->SetViewport_Size(glm::vec2(windowData->WIDTH, windowData->HEIGHT));

	//Default windows size
	renderData->renderWIDTH = windowData->WIDTH;
	renderData->renderHEIGHT = windowData->HEIGHT;
	glViewport(0, 0, renderData->renderWIDTH, renderData->renderHEIGHT);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


	//glfwSetInputMode(windowData->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(windowData->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//Blending     //ya pas de blending mm avec cette ligne au cas ou 
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Face culling     //fonctionne bizzarement ( faudrait chek l'ordre de dessin des vertex)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	//MSAA
	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

}

bool Systems::WindowSystem::ShouldClose(World& world) {
	Resource::WindowResource* ressource = world.get_ressource<Resource::WindowResource>();  // WARNING
	if (!glfwWindowShouldClose(ressource->window)) {
		return true;
	}
	return false;
}


void Systems::WindowSystem::Update(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::WindowResource* windowData = resourceBuffer->windowResource;
	ProcessInput(windowData->window);  // gere les inputs 
}


void Systems::WindowSystem::Shutdown(World& world) {
	Resource::WindowResource* Resource = world.get_ressource<Resource::WindowResource>();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	std::cout << "Window shutting down" << std::endl;
	if (Resource->window != nullptr) {
		glfwDestroyWindow(Resource->window);
		Resource->window = nullptr;
	}
	glfwTerminate();
}

