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

	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(resourceBuffer->windowResource->window, true);


	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) abort();

	//resourceBuffer->renderResource->testInterface = new bool(true);
}


void InterfaceSystem::Update(World& world, const ResourceBuffer* ressourceBuffer) {
	RenderResource* renderData = ressourceBuffer->renderResource;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGui::DockSpaceOverViewport();
	static float test;

	renderData->bloomEnable = test;

	ImGui::ShowDemoWindow();
	//// Create a window called "My First Tool", with a menu bar.
	//ImGui::Begin("My First Tool", renderData->testInterface, ImGuiWindowFlags_MenuBar);
	//if (ImGui::BeginMenuBar())
	//{
	//	if (ImGui::BeginMenu("File"))
	//	{
	//		if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
	//		if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
	//		//if (ImGui::MenuItem("Close", "Ctrl+W")) { renderData->testInterface = false; }
	//		ImGui::EndMenu();
	//	}
	//	ImGui::EndMenuBar();
	//}
	//// Edit a color stored as 4 floats
	////ImGui::ColorEdit4("Color", my_color);

	//// Display contents in a scrolling region
	//ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
	//ImGui::BeginChild("Scrolling");
	//for (int n = 0; n < 50; n++)
	//	ImGui::Text("%04d: Some text", n);
	//ImGui::EndChild();
	//ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(ressourceBuffer->windowResource->window);
}


void InterfaceSystem::Shutdown(World& world) {
	std::cout << "InterfaceSystem shutting down" << std::endl;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}