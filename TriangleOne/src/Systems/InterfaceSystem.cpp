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


void ShowExampleAppDockSpace(bool* p_open);

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

void RenderWindows(RenderResource* renderData, InterfaceRessource* interface) {
    ImGui::Begin("Render");
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    ImGui::Image(
        (ImTextureID)(intptr_t)renderData->toImGui_Texture,
        ImVec2(viewportPanelSize.x, viewportPanelSize.y),
        ImVec2(0, 1),
        ImVec2(1, 0)
    );
    ImGui::End();
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
    //ShowExampleAppDockSpace(&resourceBuffer->interfaceRessource->mainInterfaceOpen);

    //RenderWindows(renderData, interfaceData);

    ImGui::End();
    ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(resourceBuffer->windowResource->window);
}


void InterfaceSystem::Shutdown(World& world) {
	std::cout << "InterfaceSystem shutting down" << std::endl;

}


// CHANGES MADE:
// Added more clarifying comments inside the function.
// Removed MSVC warning C6011 - null pointer dereference.
// Fixed a slight grammar error - "This demo app only demonstrate" => "This demo app only demonstrates"

// Demonstrate using DockSpace() to create an explicit docking node within an existing window.
// Note: You can use most Docking facilities without calling any API. You DO NOT need to call DockSpace() to use Docking!
// - Drag from window title bar or their tab to dock/undock. Hold SHIFT to disable docking.
// - Drag from window menu button (upper-left button) to undock an entire node (all windows).
// About dockspaces:
// - Use DockSpace() to create an explicit dock node _within_ an existing window.
// - Use DockSpaceOverViewport() to create an explicit dock node covering the screen or a specific viewport.
//   This is often used with ImGuiDockNodeFlags_PassthruCentralNode.
// - Important: Dockspaces need to be submitted _before_ any window they can host. Submit it early in your frame! (*)
// - Important: Dockspaces need to be kept alive if hidden, otherwise windows docked into it will be undocked.
//   e.g. if you have multiple tabs with a dockspace inside each tab: submit the non-visible dockspaces with ImGuiDockNodeFlags_KeepAliveOnly.
// (*) because of this constraint, the implicit \"Debug\" window can not be docked into an explicit DockSpace() node,
// because that window is submitted as part of the part of the NewFrame() call. An easy workaround is that you can create
// your own implicit "Debug##2" window after calling DockSpace() and leave it in the window stack for anyone to use.
//void ShowExampleAppDockSpace(bool* p_open)
//{
//    // Variables to configure the Dockspace example.
//    static bool opt_fullscreen = true; // Is the Dockspace full-screen?
//    static bool opt_padding = false; // Is there padding (a blank space) between the window edge and the Dockspace?
//    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None; // Config flags for the Dockspace
//
//    // In this example, we're embedding the Dockspace into an invisible parent window to make it more configurable.
//    // We set ImGuiWindowFlags_NoDocking to make sure the parent isn't dockable into because this is handled by the Dockspace.
//    //
//    // ImGuiWindowFlags_MenuBar is to show a menu bar with config options. This isn't necessary to the functionality of a
//    // Dockspace, but it is here to provide a way to change the configuration flags interactively.
//    // You can remove the MenuBar flag if you don't want it in your app, but also remember to remove the code which actually
//    // renders the menu bar, found at the end of this function.
//    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
//
//    // Is the example in Fullscreen mode?
//    if (opt_fullscreen)
//    {
//        // If so, get the main viewport:
//        const ImGuiViewport* viewport = ImGui::GetMainViewport();
//
//        // Set the parent window's position, size, and viewport to match that of the main viewport. This is so the parent window
//        // completely covers the main viewport, giving it a "full-screen" feel.
//        ImGui::SetNextWindowPos(viewport->WorkPos);
//        ImGui::SetNextWindowSize(viewport->WorkSize);
//        ImGui::SetNextWindowViewport(viewport->ID);
//
//        // Set the parent window's styles to match that of the main viewport:
//        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); // No corner rounding on the window
//        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); // No border around the window
//
//        // Manipulate the window flags to make it inaccessible to the user (no titlebar, resize/move, or navigation)
//        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
//        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
//    }
//    else
//    {
//        // The example is not in Fullscreen mode (the parent window can be dragged around and resized), disable the
//        // ImGuiDockNodeFlags_PassthruCentralNode flag.
//        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
//    }
//
//    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
//    // and handle the pass-thru hole, so the parent window should not have its own background:
//    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
//        window_flags |= ImGuiWindowFlags_NoBackground;
//
//    // If the padding option is disabled, set the parent window's padding size to 0 to effectively hide said padding.
//    if (!opt_padding)
//        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
//
//    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
//    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
//    // all active windows docked into it will lose their parent and become undocked.
//    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
//    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
//    ImGui::Begin("DockSpace Demo", p_open, window_flags);
//
//    // Remove the padding configuration - we pushed it, now we pop it:
//    if (!opt_padding)
//        ImGui::PopStyleVar();
//
//    // Pop the two style rules set in Fullscreen mode - the corner rounding and the border size.
//    if (opt_fullscreen)
//        ImGui::PopStyleVar(2);
//
//    // Check if Docking is enabled:
//    ImGuiIO& io = ImGui::GetIO();
//    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
//    {
//        // If it is, draw the Dockspace with the DockSpace() function.
//        // The GetID() function is to give a unique identifier to the Dockspace - here, it's "MyDockSpace".
//        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
//        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
//    }
//    else
//    {
//        // Docking is DISABLED - Show a warning message
//        //ShowDockingDisabledMessage();
//    }
//
//    // This is to show the menu bar that will change the config settings at runtime.
//    // If you copied this demo function into your own code and removed ImGuiWindowFlags_MenuBar at the top of the function,
//    // you should remove the below if-statement as well.
//    if (ImGui::BeginMenuBar())
//    {
//        if (ImGui::BeginMenu("Options"))
//        {
//            // Disabling fullscreen would allow the window to be moved to the front of other windows,
//            // which we can't undo at the moment without finer window depth/z control.
//            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
//            ImGui::MenuItem("Padding", NULL, &opt_padding);
//            ImGui::Separator();
//
//            // Display a menu item for each Dockspace flag, clicking on one will toggle its assigned flag.
//            if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
//            if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
//            if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
//            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
//            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
//            ImGui::Separator();
//
//            // Display a menu item to close this example.
//            if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
//                if (p_open != NULL) // Remove MSVC warning C6011 (NULL dereference) - the `p_open != NULL` in MenuItem() does prevent NULL derefs, but IntelliSense doesn't analyze that deep so we need to add this in ourselves.
//                    *p_open = false; // Changing this variable to false will close the parent window, therefore closing the Dockspace as well.
//            ImGui::EndMenu();
//        }
//
//        // Show a help marker that gives an overview of what this example is and does.
//        //HelpMarker(
//        //    "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n"
//        //    "- Drag from window title bar or their tab to dock/undock." "\n"
//        //    "- Drag from window menu button (upper-left button) to undock an entire node (all windows)." "\n"
//        //    "- Hold SHIFT to disable docking." "\n"
//        //    "This demo app has nothing to do with it!" "\n\n"
//        //    "This demo app only demonstrates the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window." "\n\n"
//        //    "Read comments in ShowExampleAppDockSpace() for more details.");
//
//        ImGui::EndMenuBar();
//    }
//
//    // End the parent window that contains the Dockspace:
//    ImGui::End();
//}