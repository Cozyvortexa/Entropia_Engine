#include "Systems/InterfaceSystem.h"

namespace Resource = Engine::Resource;
namespace Component = Engine::Component;
namespace Systems = Engine::Systems;
namespace Editor = Engine::Editor;

static bool opt_fullscreen = true;

#pragma region Common function & Other
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

Entity Systems::InterfaceSystem::CreateNewEntity(World* world) {
    Entity entity = world->Register();
    Component::Transform transform(glm::vec3(0.0f));
    Component::SceneTag scene_Tag("New entity");
    world->add_components(entity, transform, scene_Tag);

    return entity;
}

// Filters the list of EntityComponentEntry objects based on a search string.
//The search is case-insensitive.
std::vector<const Editor::EntityComponentEntry*> SearchBar_GetFilter_Component(char* s_searchBuffer) {
    std::vector<const Editor::EntityComponentEntry*> filtered;
    for (const auto& entry : Editor::s_entityList) {
        if (s_searchBuffer[0] == '\0') {
            filtered.push_back(&entry);
        }
        else {
            std::string haystack = entry.name;
            std::string needle = s_searchBuffer;
            std::transform(haystack.begin(), haystack.end(), haystack.begin(), ::tolower);
            std::transform(needle.begin(), needle.end(), needle.begin(), ::tolower);
            if (haystack.find(needle) != std::string::npos)
                filtered.push_back(&entry);
        }
    }

    return filtered;
}

#pragma endregion

#pragma region Theme
static void SetTwilightPurpleTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Fonds
    colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.07f, 0.12f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.08f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.09f, 0.16f, 1.00f);

    // Bordures
    colors[ImGuiCol_Border] = ImVec4(0.35f, 0.25f, 0.45f, 0.50f);

    // Headers
    colors[ImGuiCol_Header] = ImVec4(0.40f, 0.20f, 0.60f, 0.80f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.55f, 0.30f, 0.80f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.65f, 0.35f, 0.90f, 1.00f);

    // Boutons
    colors[ImGuiCol_Button] = ImVec4(0.38f, 0.18f, 0.55f, 0.85f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.55f, 0.28f, 0.78f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.68f, 0.35f, 0.92f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.12f, 0.38f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.50f, 0.25f, 0.75f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.42f, 0.20f, 0.65f, 1.00f);

    // Titres
    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.08f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.30f, 0.15f, 0.45f, 1.00f);

    // Frame
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.11f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.18f, 0.42f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.24f, 0.55f, 1.00f);

    // Texte
    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.88f, 1.00f, 1.00f);

    // Arrondis
    style.WindowRounding = 10.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 8.0f;
}

#pragma endregion

#pragma region Render
void RenderTarget_Menu(Resource::InterfaceRessource* interfaceData) {
	if (ImGui::CollapsingHeader("RenderTarget"))
	{
		if (ImGui::RadioButton("Default render", interfaceData->renderTarget == Resource::RenderTarget::Default)) { interfaceData->renderTarget = Resource::RenderTarget::Default; }
		else if (ImGui::RadioButton("Albedo", interfaceData->renderTarget == Resource::RenderTarget::Albedo)) { interfaceData->renderTarget = Resource::RenderTarget::Albedo; }
		else if (ImGui::RadioButton("Position", interfaceData->renderTarget == Resource::RenderTarget::Position)) { interfaceData->renderTarget = Resource::RenderTarget::Position; }
		else if (ImGui::RadioButton("Normal", interfaceData->renderTarget == Resource::RenderTarget::Normal)) { interfaceData->renderTarget = Resource::RenderTarget::Normal; }
		else if (ImGui::RadioButton("Depth", interfaceData->renderTarget == Resource::RenderTarget::Depth)) { interfaceData->renderTarget = Resource::RenderTarget::Depth; }
		else if (ImGui::RadioButton("AmbientOcclusion", interfaceData->renderTarget == Resource::RenderTarget::AmbientOcclusion)) { interfaceData->renderTarget = Resource::RenderTarget::AmbientOcclusion; }
		else if (ImGui::RadioButton("Metallic", interfaceData->renderTarget == Resource::RenderTarget::Metallic)) { interfaceData->renderTarget = Resource::RenderTarget::Metallic; }
		else if (ImGui::RadioButton("Roughness", interfaceData->renderTarget == Resource::RenderTarget::Roughness)) { interfaceData->renderTarget = Resource::RenderTarget::Roughness; }
		else if (ImGui::RadioButton("Irradiance_Map", interfaceData->renderTarget == Resource::RenderTarget::Irradiance_Map)) { interfaceData->renderTarget = Resource::RenderTarget::Irradiance_Map; }
	}
}

void Display_RenderMenu(Resource::InterfaceRessource* interfaceData, Resource::RenderResource* renderData) {
    ImGui::Begin("Render settings", &interfaceData->mainInterfaceOpen, ImGuiWindowFlags_MenuBar);
    ImGuiIO& io = ImGui::GetIO();
    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "FPS: %.1f", io.Framerate);
    RenderTarget_Menu(interfaceData);

    if (ImGui::CollapsingHeader("SSAO Param"))
    {
        ImGui::InputFloat("Radius", &renderData->ssao.SSAO_radius);
        ImGui::InputInt("Sample Number", &renderData->ssao.kernelSample);
    }
    if (ImGui::CollapsingHeader("OtherParam"))
    {
        ImGui::Checkbox("bloomEnable", &renderData->bloomEnable);
        ImGui::InputFloat("Exposure", &renderData->exposure);
    }
    ImGui::End();
}

void RenderWindows(Resource::RenderResource* renderData, Resource::InterfaceRessource* interfaceData, Resource::WindowResource* windowsData) {
    ImGui::Begin("Render", &interfaceData->renderWindowsToggle);
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    ImGui::Image(
        (ImTextureID)(intptr_t)renderData->toImGui_Texture,
        ImVec2(viewportPanelSize.x, viewportPanelSize.y),
        ImVec2(0, 1),
        ImVec2(1, 0)
    );
    if (viewportPanelSize.x != interfaceData->previousSize.x || viewportPanelSize.y != interfaceData->previousSize.y) {

        renderData->renderWIDTH = (int)viewportPanelSize.x;
        renderData->renderHEIGHT = (int)viewportPanelSize.y;
        Systems::RenderSystem::ResizeFrameBufferText(renderData);
    }
    interfaceData->previousSize = viewportPanelSize;
    renderData->renderWIDTH;
    renderData->renderHEIGHT;
    ImGui::End();
}

#pragma endregion

#pragma region Hierarchy
//Create a new entity according to the user input
//If a valid entity is provided, a component will be created for that entity
void Systems::InterfaceSystem::Add_Entity_Button(World* world, Resource::RenderResource* renderData, std::string label, int entity = -1) {
    static char s_searchBuffer[128] = "";

    if (ImGui::Button(label.c_str()))
        ImGui::OpenPopup("##AddEntityPopup");

    if (ImGui::BeginPopup("##AddEntityPopup")) {

        // Search bar
        ImGui::SetNextItemWidth(220.0f);
        std::string hint = Editor::MAGNIFYING_GLASS_ICON + " Search...";
        ImGui::InputTextWithHint("##search", hint.c_str(), s_searchBuffer, sizeof(s_searchBuffer));

        ImGui::Separator();

        std::vector<const Editor::EntityComponentEntry*> filtered = SearchBar_GetFilter_Component(s_searchBuffer);


        const float itemHeight = ImGui::GetTextLineHeightWithSpacing();
        const float listHeight = itemHeight * 10.0f;  // Set the max nbr item to be display
        ImGui::BeginChild("##entityList", ImVec2(220.0f, listHeight), false, ImGuiWindowFlags_None);

        for (const auto* entry : filtered) {
            std::string label = entry->icon + "  " + entry->name;
            if (ImGui::Selectable(label.c_str())) {
                if (entity < 0) {
                    entity = (int)CreateNewEntity(world);
                }

                if (entry->type == Editor::ObjectType::EmptyObject) {

                }
                else if (entry->type == Editor::ObjectType::PointLight && !world->Has_component<Component::PointLight>(entity)) {

                    Component::PointLight pointLight(renderData->r_Shader.depthShaderCubeMap.get());
                    Component::LightToInitTag pointTag(Component::LightTag::PointLight_Tag);
                    world->add_components(entity, pointLight, pointTag);
                }
                else if (entry->type == Editor::ObjectType::DirectionalLight && !world->Has_component<Component::DirLight>(entity)) {

                    Component::DirLight dirLight(renderData->r_Shader.depthShader.get());
                    Component::LightToInitTag dirTag(Component::LightTag::Directional_Tag);
                    world->add_components(entity, dirLight, dirTag);
                }
                else if (entry->type == Editor::ObjectType::SpotLight && !world->Has_component<Component::SpotLight>(entity)) {

                    Component::SpotLight spotLight(renderData->r_Shader.depthShader.get());
                    Component::LightToInitTag spotTag(Component::LightTag::SpotLight_Tag);
                    world->add_components(entity, spotLight, spotTag);
                }
                else if (entry->type == Editor::ObjectType::Camera) {
                    // TODO
                }
                else if (entry->type == Editor::ObjectType::Mesh) {
                    // TODO
                }
                else if (entry->type == Editor::ObjectType::AudioSource) {
                    // TODO
                }
                //else if (entry->type == Editor::ObjectType::ParticuleSystem) {
                // 
                //}
                //else if (entry->type == Editor::ObjectType::Script) {

                //}
                //else if (entry->type == Editor::ObjectType::Rigidbody) {

                //}
                //else if (entry->type == Editor::ObjectType::Collider) {

                //}

                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndChild();
        ImGui::EndPopup();
    }
}

void Systems::InterfaceSystem::Display_Hierarchy_Menu(World* world, Resource::InterfaceRessource* interfaceData, Resource::RenderResource* renderData) {
    ImGui::Begin("Hierarchy", &interfaceData->hierarchy_menu);

    Add_Entity_Button(world, renderData, Editor::ADD_ICON);
    ImGui::Separator();

    View view = world->view<Component::SceneTag>();
    view.each([&](int entity, Component::SceneTag& currentSceneTag) {
        if (currentSceneTag.scene_id == 0) {
            ImGui::PushID(entity); // To avoid to have a id egall to ""
            if (ImGui::Selectable(currentSceneTag.name.c_str(), interfaceData->focusGameObject == entity)) {
                interfaceData->focusGameObject = entity;
            }
            if (ImGui::BeginPopupContextItem("ElementPopup"))
            {
                if (ImGui::MenuItem("Delete Entity"))
                {
                    world->Delete_Entity(interfaceData->focusGameObject);
                    interfaceData->focusGameObject = Engine::Component::INVALIDE_uint32_t;
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
    });

    ImGui::End();
}

#pragma endregion

#pragma region NavBar
void NavBar(Resource::RenderResource* renderData, Resource::InterfaceRessource* interfaceData) {
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
            if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
            if (ImGui::MenuItem("Close", "Ctrl+W")) { interfaceData->mainInterfaceOpen = false; }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::Checkbox("sceneObjectMenu", &interfaceData->hierarchy_menu);
            ImGui::Checkbox("renderWindowsToggle", &interfaceData->renderWindowsToggle);
            ImGui::Checkbox("inspecteur_Toogle", &interfaceData->inspecteur_Toogle);
            ImGui::Checkbox("arbo_Toogle", &interfaceData->arbo_Toogle);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

#pragma endregion 

#pragma region Inspecteur

void Systems::InterfaceSystem::Display_Inspecteur_Menu(World* world, Resource::InterfaceRessource* interfaceData, Resource::RenderResource* renderData) {
    ImGui::Begin("Inspecteur", &interfaceData->inspecteur_Toogle);
    if (interfaceData->focusGameObject != Engine::Component::INVALIDE_uint32_t) {

        //Name
        std::string* name = &world->get_component<Component::SceneTag>(interfaceData->focusGameObject)->name;
        ImGui::Text("Name: ");
        ImGui::SameLine();
        ImGui::InputText("##name", name);

        //Transform
        Component::Transform* currentTransform = world->get_component<Component::Transform>(interfaceData->focusGameObject);
        if (currentTransform != nullptr && ImGui::CollapsingHeader("Transform")) {


            ImGui::PushID(interfaceData->focusGameObject + "Transform");
            Editor::DrawComponentUI(*currentTransform);
            ImGui::PopID();
        }

        ////Light
        //PointLight
        Editor::DrawComponentSection<Component::PointLight>(world, interfaceData->focusGameObject, "Point light");

        //DirLight
        Editor::DrawComponentSection<Component::DirLight>(world, interfaceData->focusGameObject, "Directional Light");

        //SpotLight
        Editor::DrawComponentSection<Component::SpotLight>(world, interfaceData->focusGameObject, "Spot light");

        std::string label = "Add new Component";
        Add_Entity_Button(world, renderData, label, (int)interfaceData->focusGameObject);
    }
    ImGui::End();
}

#pragma endregion

#pragma region Arbo
Resource::FileType Systems::InterfaceSystem::GetType(const std::filesystem::path& path) {
    if (std::filesystem::is_directory(path))
        return Resource::FileType::Directory;

    std::string ext = path.extension().string();

    if (ext == ".mp3" || ext == ".wav")
        return Resource::FileType::Audio;

    if (ext == ".mp4" || ext == ".mkv")
        return Resource::FileType::Video;

    if (ext == ".png" || ext == ".jpg")
        return Resource::FileType::Image;

    if (ext == ".fbx" || ext == ".obj" || ext == "glb")
        return Resource::FileType::Model;

    return Resource::FileType::Other;
}

std::unique_ptr<Resource::Node> Systems::InterfaceSystem::BuildTree(const std::filesystem::path& rootPath, Resource::Node* parent) {
    auto node = std::make_unique<Resource::Node>();
    node->name = rootPath.filename().string();
    node->path = rootPath.string();
    node->parent = parent;

    if (std::filesystem::is_directory(rootPath)) {
        node->type = Resource::FileType::Directory;

        for (const auto& entry : std::filesystem::directory_iterator(rootPath)) {
            node->children.push_back(BuildTree(entry.path(), node.get()));
        }
    }
    else {
        node->type = GetType(rootPath);
    }

    return node;
}

// Navigation bar (breadcrumb) 
// Reconstructs the path from the root to the current folder and display it
void inline Arbo_Breadcrum(Resource::InterfaceRessource* interfaceData) {
    std::vector<Resource::Node*> breadcrumb;

    Resource::Node* cursor = interfaceData->focusDirectory;
    while (cursor != nullptr && cursor != cursor->parent) {
        breadcrumb.push_back(cursor);
        cursor = cursor->parent;
    }
    // Add the root if it is not already included
    if (cursor != nullptr && (breadcrumb.empty() || breadcrumb.back() != cursor))
        breadcrumb.push_back(cursor);
    std::reverse(breadcrumb.begin(), breadcrumb.end());


    // Displays the breadcrumb buttons on a single line
    for (size_t i = 0; i < breadcrumb.size(); ++i) {
        if (i > 0) {
            ImGui::SameLine();
            ImGui::TextDisabled("/");
            ImGui::SameLine();
        }
        ImGui::PushID(static_cast<int>(i));
        // Last item = current folder, highlighted
        if (i == breadcrumb.size() - 1) {
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", breadcrumb[i]->name.c_str());
        }
        else {
            if (ImGui::SmallButton(breadcrumb[i]->name.c_str())) {
                interfaceData->focusDirectory = breadcrumb[i];
            }
        }
        ImGui::PopID();
    }
}

// Refresh button
void inline Systems::InterfaceSystem::Arbo_RefreshButton(Resource::InterfaceRessource* interfaceData) {
    std::string refreshLabel = std::string(ICON_FA_ARROWS_ROTATE) + " ";
    float buttonWidth = ImGui::CalcTextSize(refreshLabel.c_str()).x + ImGui::GetStyle().FramePadding.x * 2.0f;
    float available = ImGui::GetContentRegionAvail().x;

    ImGui::SameLine(available - buttonWidth + ImGui::GetStyle().WindowPadding.x);
    if (ImGui::Button(refreshLabel.c_str())) {
        interfaceData->mainDirectory = BuildTree("Assets");
        interfaceData->focusDirectory = interfaceData->mainDirectory.get();
    }
}

std::string inline FileTypeToIcon(Resource::FileType type) {
    std::string icon;
    if (type== Resource::FileType::Directory)
        icon = Editor::FOLDER_ICON;
    else if (type== Resource::FileType::Image)
        icon = ICON_FA_IMAGE;
    else if (type== Resource::FileType::Audio)
        icon = Editor::MUSIC_ICON;
    else if (type== Resource::FileType::Model)
        icon = Editor::DICE_D6_ICON;
    else if (type== Resource::FileType::Video)
        icon = ICON_FA_FILM;
    else
        icon = ICON_FA_FILE;

    return icon;
}

void Systems::InterfaceSystem::Display_ArboMenu(Resource::InterfaceRessource* interfaceData) {
    ImGui::Begin("Arbo", &interfaceData->arbo_Toogle);
    ImGui::SetWindowFontScale(1.5f);

    Arbo_Breadcrum(interfaceData);

    Arbo_RefreshButton(interfaceData);

    ImGui::Separator();

    // File/folder grid 
    const float iconButtonSize = 80.0f;
    float windowWidth = ImGui::GetContentRegionAvail().x;
    int   columns = std::max(1, static_cast<int>(windowWidth / iconButtonSize));

    int itemIndex = 0;
    for (const auto& currentNode_unique_ptr : interfaceData->focusDirectory->children) {
        Resource::Node* currentNode = currentNode_unique_ptr.get();

        // Automatic line break based on the number of columns
        if (itemIndex > 0 && itemIndex % columns != 0)
            ImGui::SameLine();

        ImGui::PushID(currentNode);
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(60, 120, 255, 0));


        std::string icon = FileTypeToIcon(currentNode->type);

        // Truncate the name if it is too long to fit in the button
        std::string displayName = currentNode->name;
        if (displayName.size() > 12)
            displayName = displayName.substr(0, 12) + "..";

        std::string label = icon + "\n" + displayName;

        if (ImGui::Button(label.c_str(), ImVec2(iconButtonSize, iconButtonSize))) {
            if (currentNode->type == Resource::FileType::Directory) {
                interfaceData->focusDirectory = currentNode;
            }
            else if (currentNode->type == Resource::FileType::Image) {
                //TEMP
                ShellExecuteA(
                    NULL,
                    "open",
                    currentNode->path.c_str(),
                    NULL,
                    NULL,
                    SW_SHOWNORMAL
                );

            }
            else if (currentNode->type == Resource::FileType::Audio) {
                // TODO
            }
            else if (currentNode->type == Resource::FileType::Model) {
                // TODO
            }
        }

        // Dispaly item name on Hover
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", currentNode->name.c_str());

        ImGui::PopStyleColor(1);
        ImGui::PopID();

        ++itemIndex;
    }

    ImGui::SetWindowFontScale(1.0f);
    ImGui::End();
}

#pragma endregion

void Systems::InterfaceSystem::Init(World& world, const Resource::ResourceBuffer* resourceBuffer) {
    Resource::InterfaceRessource* interfaceData = resourceBuffer->interfaceRessource;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
    io.DeltaTime = true;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    ////Icon & Font
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;
    ImFont* font = io.Fonts->AddFontFromFileTTF("Assets/Font/Amarna-VariableFont_wght.ttf");
    io.Fonts->AddFontFromFileTTF("Assets/Font/Font_Awesome_7_Free-Solid-900.otf", 16.0f, &config, icons_ranges);


    ////

    ImGui_ImplOpenGL3_Init();
    ImGui_ImplGlfw_InitForOpenGL(resourceBuffer->windowResource->window, true);

    SetTwilightPurpleTheme();

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) abort();

    interfaceData->OnEditorView = true;

    interfaceData->mainDirectory = BuildTree("Assets", interfaceData->mainDirectory.get());
    interfaceData->focusDirectory = interfaceData->mainDirectory.get();
}

void Systems::InterfaceSystem::Update(World& world, const Resource::ResourceBuffer* resourceBuffer) {
	Resource::RenderResource* renderData = resourceBuffer->renderResource;
	Resource::InterfaceRessource* interfaceData = resourceBuffer->interfaceRessource;
	Resource::WindowResource* windowsData = resourceBuffer->windowResource;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Set the parent window's position, size, and viewport to match that of the main viewport. This is so the parent window
    // completely covers the main viewport, giving it a "full-screen" feel.
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::Begin("Main Page", &resourceBuffer->interfaceRessource->mainInterfaceOpen, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | 
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking);
    NavBar(renderData, interfaceData);


    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(
        dockspace_id,
        ImVec2(0.0f, 0.0f),             
        ImGuiDockNodeFlags_PassthruCentralNode
    );


    RenderWindows(renderData, interfaceData, windowsData);
    Display_RenderMenu(interfaceData, renderData);
    Display_Hierarchy_Menu(&world, interfaceData, renderData);
    Display_Inspecteur_Menu(&world, interfaceData, renderData);
    Display_ArboMenu(interfaceData);



    ImGui::End();
    ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(resourceBuffer->windowResource->window);
}

void Systems::InterfaceSystem::Shutdown(World& world) {
	std::cout << "InterfaceSystem shutting down" << std::endl;
}
