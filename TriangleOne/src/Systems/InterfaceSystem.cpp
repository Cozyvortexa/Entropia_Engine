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

template<typename T>
std::vector<const T*> SearchBar_GetFilter(const std::vector<T>& sourceList, const char* s_searchBuffer) {
    std::vector<const T*> filtered;

    if (s_searchBuffer == nullptr || s_searchBuffer[0] == '\0') {
        for (const auto& entry : sourceList) {
            filtered.push_back(&entry);
        }
        return filtered;
    }

    std::string needle = s_searchBuffer;
    std::transform(needle.begin(), needle.end(), needle.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    for (const auto& entry : sourceList) {
        std::string haystack = entry.name;
        std::transform(haystack.begin(), haystack.end(), haystack.begin(), [](unsigned char c) {
            return std::tolower(c);
            });

        if (haystack.find(needle) != std::string::npos) {
            filtered.push_back(&entry);
        }
    }

    return filtered;
}

//Create a new entity according to the user input
//If a valid entity is provided, a component will be created for that entity
void Systems::InterfaceSystem::Add_Entity_Button(World* world, Resource::RenderResource* renderData, Resource::InterfaceRessource* interfaceData, Resource::AudioResource* audioData, std::string label, int entity = -1) {
    static char s_searchBuffer[128] = "";

    if (ImGui::Button(label.c_str()))
        ImGui::OpenPopup("##AddEntityPopup");

    if (ImGui::BeginPopup("##AddEntityPopup")) {

        // Search bar
        ImGui::SetNextItemWidth(220.0f);
        std::string hint = Editor::MAGNIFYING_GLASS_ICON + " Search...";
        ImGui::InputTextWithHint("##search", hint.c_str(), s_searchBuffer, sizeof(s_searchBuffer));

        ImGui::Separator();

        std::vector<const Editor::EntityComponentEntry*> filtered = SearchBar_GetFilter(Editor::s_entityList, s_searchBuffer);


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
                    Component::MeshHandle meshHandle(-1);
                    Component::MaterialHandle materialHandle(renderData->mainMaterialHandle);
                    world->add_components(entity, meshHandle, materialHandle);
                }
                else if (entry->type == Editor::ObjectType::AudioSource) {
                    Component::AudioSource audioSource;
                    world->add_components(entity, std::move(audioSource));
                }
                else if (entry->type == Editor::ObjectType::Collider) {
                    Component::BoxCollider boxCollider(world->get_component<Component::Transform>(entity)->position);
                    world->add_components(entity, std::move(boxCollider));
                }
                //else if (entry->type == Editor::ObjectType::ParticuleSystem) {
                // 
                //}
                //else if (entry->type == Editor::ObjectType::Script) {

                //}
                //else if (entry->type == Editor::ObjectType::Rigidbody) {

                //}

                if (interfaceData->focusGameObject != entity) interfaceData->focusGameObject = entity;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndChild();
        ImGui::EndPopup();
    }
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

void Display_RenderMenu(Resource::InterfaceRessource* interfaceData, Resource::RenderResource* renderData, Component::CameraComponent* cameraComponent, Resource::PhysicsResource* physicsData) {
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
        ImGui::InputFloat("Camera speed", &cameraComponent->cameraSpeed);
        ImGui::Checkbox("Display Collider Box", &physicsData->displayColliderBox);
    }
    ImGui::End();
}

void RenderWindows(World* word, Resource::RenderResource* renderData, Resource::InterfaceRessource* interfaceData, Resource::WindowResource* windowData) {
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
        if (!windowData->isIconified) word->renderer->ResizeFrameBufferText(renderData);
    }
    interfaceData->previousSize = viewportPanelSize;
    renderData->renderWIDTH;
    renderData->renderHEIGHT;
    ImGui::End();
}

#pragma endregion

#pragma region Hierarchy

void Systems::InterfaceSystem::Display_Hierarchy_Menu(World* world, const Resource::ResourceBuffer* resourceBuffer) {
    Resource::InterfaceRessource* interfaceData = resourceBuffer->interfaceRessource;
    Resource::RenderResource* renderData = resourceBuffer->renderResource;
    Resource::AudioResource* audioData = resourceBuffer->audioResource;


    ImGui::Begin("Hierarchy", &interfaceData->hierarchy_menu);

    Add_Entity_Button(world, renderData, interfaceData, audioData, Editor::ADD_ICON);
    ImGui::SameLine();
    
    // Search bar
    static char s_searchBuffer[128] = "";
    ImGui::SetNextItemWidth(120.0f);
    std::string hint = Editor::MAGNIFYING_GLASS_ICON + " Search...";
    ImGui::InputTextWithHint("##search_Hierarchy", hint.c_str(), s_searchBuffer, sizeof(s_searchBuffer));
    ImGui::Separator();

    bool hasSearchFilter = (s_searchBuffer != nullptr && s_searchBuffer[0] != '\0');
    std::string needle = "";
    if (hasSearchFilter) {
        needle = s_searchBuffer;
        std::transform(needle.begin(), needle.end(), needle.begin(), [](unsigned char c) { return std::tolower(c);});
    }

    bool deleteFocus = false;
    View view = world->view<Component::SceneTag>();
    view.each([&](int entity, Component::SceneTag& currentSceneTag) {
        if (currentSceneTag.scene_id == 0) {

            if (hasSearchFilter) {
                std::string haystack = currentSceneTag.name;
                std::transform(haystack.begin(), haystack.end(), haystack.begin(), [](unsigned char c) {
                    return std::tolower(c);
                    });

                if (haystack.find(needle) == std::string::npos) {
                    return;
                }
            }

            ImGui::PushID(entity); // To avoid to have a id egall to ""
            if (ImGui::Selectable(currentSceneTag.name.c_str(), interfaceData->focusGameObject == entity)) {
                interfaceData->focusGameObject = entity;
            }
            if (ImGui::BeginPopupContextItem("ElementPopup"))
            {
                if (ImGui::MenuItem("Delete Entity")){
                    deleteFocus = true;
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
    });

    if (deleteFocus) {
        world->Delete_Entity(interfaceData->focusGameObject);
        interfaceData->focusGameObject = Engine::Component::INVALIDE_uint32_t;
    }

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

        if (ImGui::BeginMenu("Theme")) {
            if (ImGui::MenuItem("TwilightPurple")) Editor::SetTwilightPurpleTheme();
            if (ImGui::MenuItem("MidnightCarbon")) Editor::SetMidnightCarbonTheme();
            if (ImGui::MenuItem("EmberForge")) Editor::SetEmberForgeTheme();
            if (ImGui::MenuItem("ArcticSlate")) Editor::SetArcticSlateTheme();
            if (ImGui::MenuItem("DeepForest")) Editor::SetDeepForestTheme();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

#pragma endregion 

#pragma region Inspecteur

void Systems::InterfaceSystem::Display_Inspecteur_Menu(World* world, const Resource::ResourceBuffer* resourceBuffer) {
    Resource::InterfaceRessource* interfaceData = resourceBuffer->interfaceRessource;
    Resource::AudioResource* audioData = resourceBuffer->audioResource;
    Resource::RenderResource* renderData = resourceBuffer->renderResource;


    ImGui::Begin("Inspecteur", &interfaceData->inspecteur_Toogle);
    if (interfaceData->focusGameObject != Engine::Component::INVALIDE_uint32_t) {

        //Name
        std::string* name = &world->get_component<Component::SceneTag>(interfaceData->focusGameObject)->name;
        ImGui::Text("Name: ");
        ImGui::SameLine();
        ImGui::InputText("##name", name);

        Editor::EditorContext editorContext{ world, resourceBuffer };

        //Transform
        Component::Transform* currentTransform = world->get_component<Component::Transform>(interfaceData->focusGameObject);
        assert(currentTransform != nullptr && " Scene object without transform detected in interface system");

        if (ImGui::CollapsingHeader("Transform")) {

            ImGui::PushID(interfaceData->focusGameObject + "Transform");
            Editor::DrawComponentUI(*currentTransform, editorContext);
            ImGui::PopID();
        }

        ////Light
        //PointLight
        Editor::DrawComponentSection<Component::PointLight>(editorContext, interfaceData->focusGameObject, "Point light");

        //DirLight
        Editor::DrawComponentSection<Component::DirLight>(editorContext, interfaceData->focusGameObject, "Directional Light");

        //SpotLight
        Editor::DrawComponentSection<Component::SpotLight>(editorContext, interfaceData->focusGameObject, "Spot light");

        //Audio Source
        Editor::DrawComponentSection<Component::AudioSource>(editorContext, interfaceData->focusGameObject, "Audio source");

        //Mesh
        Editor::DrawComponentSection<Component::MeshHandle>(editorContext, interfaceData->focusGameObject, "Mesh");

        //BoxCollider
        Editor::DrawComponentSection<Component::BoxCollider>(editorContext, interfaceData->focusGameObject, "BoxCollider");

        ImGui::Separator();
        std::string label = "Add new Component";
        Add_Entity_Button(world, renderData, interfaceData, audioData, label, (int)interfaceData->focusGameObject);
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

    if (ext == ".fbx" || ext == ".obj" || ext == ".glb")
        return Resource::FileType::Model;

    return Resource::FileType::Other;
}

std::unique_ptr<Resource::Node> Systems::InterfaceSystem::BuildTree(const std::filesystem::path& rootPath, Resource::Node* parent) {
    auto node = std::make_unique<Resource::Node>();
    node->name = rootPath.filename().string();
    node->path = rootPath.generic_string();
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
    int columns = std::max(1, static_cast<int>(windowWidth / iconButtonSize));

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
            }
            else if (currentNode->type == Resource::FileType::Model) {
                // TODO
            }
        }

        // Dispaly item name on Hover
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", currentNode->name.c_str());

        //Drag Object
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("ARBO_ITEM", &currentNode, sizeof(Engine::Resource::Node));

            ImGui::Text(label.c_str());

            ImGui::EndDragDropSource();
        }



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

    Editor::SetTwilightPurpleTheme();

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
	Resource::PhysicsResource* physicsData = resourceBuffer->physicsResource;

    Entity entityCam = resourceBuffer->activeCamera->cameraID;
    Component::CameraComponent* mainCamera = world.get_component<Component::CameraComponent>(entityCam);

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


    RenderWindows(&world, renderData, interfaceData, windowsData);
    Display_RenderMenu(interfaceData, renderData, mainCamera, physicsData);
    Display_Hierarchy_Menu(&world, resourceBuffer);
    Display_Inspecteur_Menu(&world, resourceBuffer);
    Display_ArboMenu(interfaceData);



    ImGui::End();
    ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(resourceBuffer->windowResource->window);
}

void Systems::InterfaceSystem::Shutdown(World& world) {
	std::cout << "InterfaceSystem shutting down" << std::endl;
}
