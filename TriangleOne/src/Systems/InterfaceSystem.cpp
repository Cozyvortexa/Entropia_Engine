#include "Systems/InterfaceSystem.h"

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void InterfaceSystem::Init(World& world, const ResourceBuffer* resourceBuffer) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
	io.DeltaTime = true;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(resourceBuffer->windowResource->window, true);

	resourceBuffer->interfaceRessource->mainInterfaceOpen = true;

	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) abort();

	//resourceBuffer->renderResource->testInterface = new bool(true);
}

void RenderTarget_Menu(InterfaceRessource* interface) {
	if (ImGui::CollapsingHeader("RenderTarget"))
	{
		if (ImGui::RadioButton("Default render", interface->renderTarget == RenderTarget::Default)) { interface->renderTarget = RenderTarget::Default; }
		else if (ImGui::RadioButton("Albedo", interface->renderTarget == RenderTarget::Albedo)) { interface->renderTarget = RenderTarget::Albedo; }
		else if (ImGui::RadioButton("Specular", interface->renderTarget == RenderTarget::Specular)) { interface->renderTarget = RenderTarget::Specular; }
		else if (ImGui::RadioButton("Position", interface->renderTarget == RenderTarget::Position)) { interface->renderTarget = RenderTarget::Position; }
		else if (ImGui::RadioButton("Normal", interface->renderTarget == RenderTarget::Normal)) { interface->renderTarget = RenderTarget::Normal; }
		else if (ImGui::RadioButton("Depth", interface->renderTarget == RenderTarget::Depth)) { interface->renderTarget = RenderTarget::Depth; }
		else if (ImGui::RadioButton("AmbientOcclusion", interface->renderTarget == RenderTarget::AmbientOcclusion)) { interface->renderTarget = RenderTarget::AmbientOcclusion; }
	}
}

void InterfaceSystem::Update(World& world, const ResourceBuffer* resourceBuffer) {
	RenderResource* renderData = resourceBuffer->renderResource;
	InterfaceRessource* interfaceData = resourceBuffer->interfaceRessource;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGui::DockSpaceOverViewport();

	//ImGui::ShowDemoWindow();
	// Create a window called "My First Tool", with a menu bar.
	ImGui::Begin("Main Page", &resourceBuffer->interfaceRessource->mainInterfaceOpen, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
			if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
			if (ImGui::MenuItem("Close", "Ctrl+W")) { resourceBuffer->interfaceRessource->mainInterfaceOpen = false; }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGuiIO& io = ImGui::GetIO();
	ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "FPS: %.1f", io.Framerate);

	RenderTarget_Menu(interfaceData);

	if (ImGui::CollapsingHeader("OtherParam"))
	{
		ImGui::Checkbox("bloomEnable", &renderData->bloomEnable);
	}


	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(resourceBuffer->windowResource->window);
}


void InterfaceSystem::Shutdown(World& world) {
	std::cout << "InterfaceSystem shutting down" << std::endl;

}