#pragma once
#include <string>
#include <vector>
#include "Utilities/Font_awesome.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ECS/Components/Component.h"

#include "ECS/World.h"
#include "Utilities/Observer.h"

namespace Component = Engine::Component;

namespace Engine::Editor {
    enum class ObjectType {
        EmptyObject,
        PointLight,
        DirectionalLight,
        SpotLight,
        Camera,
        Mesh,
        AudioSource,
        //ParticuleSystem,
        //Script,
        //Rigidbody,
        //Collider,
    };

    struct EntityComponentEntry {
        std::string icon;
        std::string name;
        ObjectType type;
    };

    struct EditorContext {
        World* world;
        const Resource::ResourceBuffer* resourceBuffer;
    };

#pragma region PrimitiveType
    template<typename T>
    inline void DrawWidget(EditorContext ctx, const char* label, T& value) {
        std::cout << "The method for displaying the label: " << label << " is unknown" << std::endl; // Fallback
    };  

    template<>
    inline void DrawWidget<int>(EditorContext ctx, const char* label, int& value) {
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputInt(("##" + std::string(label)).c_str(), &value);
    }
    template<>
    inline void DrawWidget<float>(EditorContext ctx, const char* label, float& value) {
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputFloat(("##" + std::string(label)).c_str(), &value);
    }
    template<>
    inline void DrawWidget<double>(EditorContext ctx, const char* label, double& value) {
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputDouble(("##" + std::string(label)).c_str(), &value);
    }
    template<>
    inline void DrawWidget<bool>(EditorContext ctx, const char* label, bool& value) {
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::Checkbox(("##" + std::string(label)).c_str(), &value);
    }
    template<>
    inline void DrawWidget<glm::vec2>(EditorContext ctx, const char* label, glm::vec2& value) {
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputFloat2(("##" + std::string(label)).c_str(), &value.x);
    }
    template<>
    inline void DrawWidget<glm::vec3>(EditorContext ctx, const char* label, glm::vec3& value) {
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputFloat3(("##" + std::string(label)).c_str(), &value.x);
    }
    template<>
    inline void DrawWidget<glm::vec4>(EditorContext ctx, const char* label, glm::vec4& value) {
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputFloat4(("##" + std::string(label)).c_str(), &value.x);
    }
    template<>
    inline void DrawWidget<std::string>(EditorContext ctx, const char* label, std::string& value) {
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText(("##" + std::string(label)).c_str(), &value);
    }

#pragma endregion

#pragma region Observer
    template<>
    inline void DrawWidget<Observer<int>>(EditorContext ctx, const char* label, Observer<int>& value) {
        int temp = value.Get();

        if (ImGui::InputInt(label, &temp))
        {
            value.Set(temp);
        }
    }
    template<>
    inline void DrawWidget<Observer<float>>(EditorContext ctx, const char* label, Observer<float>& value) {
        float temp = value.Get();

        if (ImGui::InputFloat(label, &temp))
        {
            value.Set(temp);
        }
    }

#pragma endregion 

#pragma region CustomClass
    template<>
    inline void DrawWidget<Engine::Audio::Audio*>(EditorContext ctx, const char* label, Engine::Audio::Audio*& value) {
        DrawWidget<std::string>(ctx, "Name: ", value->name);

        DrawWidget<bool>(ctx, "Looping", value->looping);
        DrawWidget<bool>(ctx, "Spatialisation", value->spatialisation);

        if (ImGui::Button(ICON_FA_PLAY)) {ctx.world->audioManager->Start(*value);}
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_PAUSE)) {ctx.world->audioManager->Pause(*value);}

        ImGui::Button("Drag and Drop new song here");
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ARBO_ITEM"))
            {
                Engine::Resource::Node* node = *static_cast<Engine::Resource::Node**>(payload->Data);
                if (node->type == Engine::Resource::FileType::Audio) {
                    Engine::Audio::SoundFlags flags = value->flags;
                    Engine::Audio::AudioManager::DeleteSound(value);

                    value = ctx.world->assetStore->Load_Sound(ctx.resourceBuffer->audioResource->audioEngine, node->name, node->path.c_str(), flags);
                }

            }

            ImGui::EndDragDropTarget();
        }
    }
    template<>
    inline void DrawWidget<Engine::Component::MeshIndex>(EditorContext ctx, const char* label, Engine::Component::MeshIndex& value) {
        Mesh mesh;
        if (value == -1) {
            std::string emptypath = "None";
            DrawWidget<std::string>(ctx, "Directory", emptypath);
        }
        else {
            Mesh mesh = ctx.world->assetStore->Get_Mesh(value);
            DrawWidget<std::string>(ctx, "Directory", mesh.directory);
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ARBO_ITEM"))
            {
                Engine::Resource::Node* node = *static_cast<Engine::Resource::Node**>(payload->Data);
                if (node->type == Engine::Resource::FileType::Model) {

                    std::pair<Mesh&, int> newMesh = ctx.world->assetStore->Get_Mesh(node->path);
                    value = newMesh.second;
                }
            }

            ImGui::EndDragDropTarget();
        }


        ImGui::BeginDisabled();
        ImGui::AlignTextToFramePadding();
        DrawWidget<bool>(ctx, "Have Normal map:", mesh.hasNormalMap);
        ImGui::SameLine();
        DrawWidget<bool>(ctx, "Have TangentBitangent:", mesh.hasTBN);
        ImGui::SameLine();
        DrawWidget<bool>(ctx, "Have UV map:", mesh.hasUV);
        ImGui::EndDisabled();
    }

#pragma endregion

    template<typename T>
    inline void DrawComponentUI(T& component, EditorContext ctx)
    {
        component.Reflect([&](const char* name, auto& value){
                Engine::Editor::DrawWidget(ctx, name, value);
            });
    }

    template<typename T>
    bool DrawComponentSection(EditorContext ctx, Entity entity, const char* label)
    {
        T* component = ctx.world->get_component<T>(entity);

        if (!component)
            return false;

        ImGui::PushID(label);

        bool open = ImGui::CollapsingHeader(label);

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Component"))
            {
                ctx.world->remove_component<T>(entity);

                ImGui::EndPopup();
                ImGui::PopID();
                return false;
            }

            ImGui::EndPopup();
        }

        if (open)
        {
            Editor::DrawComponentUI(*component, ctx);
        }

        ImGui::PopID();

        return true;
    }

    static const std::vector<EntityComponentEntry> s_entityList = {
        { ICON_FA_CUBE,       "Empty Object",      ObjectType::EmptyObject      },
        { ICON_FA_LIGHTBULB,  "Point Light",       ObjectType::PointLight       },
        { ICON_FA_SUN,        "Directional Light", ObjectType::DirectionalLight },
        { ICON_FA_CIRCLE_DOT, "Spot Light",        ObjectType::SpotLight        },
        { ICON_FA_VIDEO,      "Camera",            ObjectType::Camera           },
        { ICON_FA_DICE_D6,    "Mesh",              ObjectType::Mesh             },
        { ICON_FA_VOLUME_HIGH, "Audio Source",     ObjectType::AudioSource      },
        /*{ ICON_FA_WIND,       "Particle System", ObjectType::ParticuleSystem  },*/
        /*{ ICON_FA_CODE,       "Script",          ObjectType::Script           },*/
        /*{ ICON_FA_MAGNET,     "Rigidbody",       ObjectType::Rigidbody        },*/
        /*{ ICON_FA_BORDER_ALL, "Collider",        ObjectType::Collider         },*/
    };


#pragma region Theme
    static void SetTwilightPurpleTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // === FONDS ===
        colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.07f, 0.12f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.08f, 0.14f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.09f, 0.16f, 1.00f);

        // === BORDURES ===
        colors[ImGuiCol_Border] = ImVec4(0.35f, 0.25f, 0.50f, 0.60f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        // === TEXTE ===
        colors[ImGuiCol_Text] = ImVec4(0.92f, 0.88f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.42f, 0.62f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.50f, 0.25f, 0.75f, 0.40f);

        // === FRAMES (inputs, sliders, checkboxes) ===
        colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.11f, 0.22f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.18f, 0.42f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.24f, 0.55f, 1.00f);

        // === TITRES ===
        colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.07f, 0.15f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.28f, 0.14f, 0.42f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.06f, 0.13f, 0.75f);

        // === MENU BAR ===
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.08f, 0.18f, 1.00f);

        // === SCROLLBAR ===
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.09f, 0.07f, 0.12f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.38f, 0.20f, 0.55f, 0.80f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.28f, 0.72f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.65f, 0.35f, 0.88f, 1.00f);

        // === CHECKMARK / RADIO ===
        colors[ImGuiCol_CheckMark] = ImVec4(0.75f, 0.45f, 1.00f, 1.00f);

        // === SLIDERS ===
        colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.28f, 0.80f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.70f, 0.38f, 0.95f, 1.00f);

        // === BOUTONS ===
        colors[ImGuiCol_Button] = ImVec4(0.38f, 0.18f, 0.55f, 0.85f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.55f, 0.28f, 0.78f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.68f, 0.35f, 0.92f, 1.00f);

        // === HEADERS (TreeNode, Selectable, CollapsingHeader) ===
        colors[ImGuiCol_Header] = ImVec4(0.38f, 0.18f, 0.56f, 0.80f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.52f, 0.28f, 0.76f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.62f, 0.34f, 0.88f, 1.00f);

        // === SÉPARATEUR ===
        colors[ImGuiCol_Separator] = ImVec4(0.35f, 0.22f, 0.50f, 0.60f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.55f, 0.30f, 0.78f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.68f, 0.38f, 0.92f, 1.00f);

        // === RESIZE GRIP ===
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.45f, 0.22f, 0.65f, 0.40f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.58f, 0.30f, 0.82f, 0.70f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.72f, 0.40f, 0.96f, 1.00f);

        // === TABS ===
        colors[ImGuiCol_Tab] = ImVec4(0.22f, 0.11f, 0.34f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.50f, 0.25f, 0.74f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.40f, 0.20f, 0.62f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.08f, 0.20f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.28f, 0.14f, 0.42f, 1.00f);

        // === DOCKING ===
        colors[ImGuiCol_DockingPreview] = ImVec4(0.60f, 0.30f, 0.90f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.08f, 0.06f, 0.11f, 1.00f);

        // === PLOT ===
        colors[ImGuiCol_PlotLines] = ImVec4(0.70f, 0.50f, 0.90f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.65f, 1.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.60f, 0.35f, 0.85f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.80f, 0.50f, 1.00f, 1.00f);

        // === TABLE ===
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18f, 0.10f, 0.28f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.40f, 0.22f, 0.58f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.14f, 0.38f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);

        // === DRAG & DROP ===
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.80f, 0.45f, 1.00f, 0.90f);

        // === NAVIGATION ===
        colors[ImGuiCol_NavHighlight] = ImVec4(0.70f, 0.38f, 1.00f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.08f, 0.05f, 0.12f, 0.60f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.06f, 0.04f, 0.10f, 0.65f);

        // === STYLE ===
        style.WindowRounding = 10.0f;
        style.ChildRounding = 8.0f;
        style.FrameRounding = 6.0f;
        style.PopupRounding = 8.0f;
        style.GrabRounding = 6.0f;
        style.TabRounding = 8.0f;
        style.ScrollbarRounding = 8.0f;
        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.ScrollbarSize = 12.0f;
        style.GrabMinSize = 10.0f;
    }

    static void SetMidnightCarbonTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.04f, 0.06f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.05f, 0.05f, 0.08f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.07f, 0.10f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.12f, 0.22f, 0.28f, 0.80f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_Text] = ImVec4(0.85f, 0.95f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.30f, 0.40f, 0.45f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.02f, 0.60f, 0.75f, 0.35f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.12f, 0.16f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.22f, 0.28f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.05f, 0.32f, 0.42f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.03f, 0.04f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.04f, 0.18f, 0.25f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.03f, 0.04f, 0.06f, 0.75f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.04f, 0.06f, 0.09f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.03f, 0.04f, 0.06f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.05f, 0.35f, 0.50f, 0.80f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.05f, 0.52f, 0.68f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.10f, 0.70f, 0.88f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.10f, 0.80f, 0.95f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.08f, 0.55f, 0.72f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.12f, 0.72f, 0.90f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.05f, 0.28f, 0.38f, 0.90f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.06f, 0.42f, 0.58f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.08f, 0.58f, 0.76f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.05f, 0.28f, 0.38f, 0.80f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.06f, 0.40f, 0.55f, 0.90f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.08f, 0.55f, 0.72f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.10f, 0.25f, 0.32f, 0.70f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.08f, 0.50f, 0.65f, 0.80f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.68f, 0.86f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.05f, 0.35f, 0.50f, 0.40f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.06f, 0.52f, 0.68f, 0.70f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.08f, 0.70f, 0.88f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.04f, 0.14f, 0.20f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.06f, 0.40f, 0.56f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.05f, 0.30f, 0.44f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.03f, 0.08f, 0.12f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.04f, 0.18f, 0.26f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.08f, 0.65f, 0.85f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.03f, 0.04f, 0.06f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.10f, 0.75f, 0.90f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.20f, 0.95f, 1.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.08f, 0.60f, 0.78f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.15f, 0.85f, 1.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.04f, 0.14f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.08f, 0.28f, 0.38f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.06f, 0.18f, 0.26f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.10f, 0.80f, 1.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.10f, 0.75f, 0.95f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.02f, 0.03f, 0.05f, 0.65f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.02f, 0.02f, 0.04f, 0.70f);

        style.WindowRounding = 4.0f;
        style.ChildRounding = 4.0f;
        style.FrameRounding = 3.0f;
        style.PopupRounding = 4.0f;
        style.GrabRounding = 3.0f;
        style.TabRounding = 4.0f;
        style.ScrollbarRounding = 4.0f;
        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 5.0f);
        style.ScrollbarSize = 10.0f;
        style.GrabMinSize = 8.0f;
    }

    static void SetEmberForgeTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.05f, 0.03f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.06f, 0.04f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.07f, 0.04f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.38f, 0.22f, 0.08f, 0.70f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_Text] = ImVec4(0.98f, 0.90f, 0.80f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.48f, 0.36f, 0.24f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.72f, 0.32f, 0.05f, 0.40f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.09f, 0.05f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.16f, 0.06f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.42f, 0.22f, 0.05f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.04f, 0.02f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.16f, 0.04f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.04f, 0.02f, 0.75f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.06f, 0.03f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.04f, 0.02f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.50f, 0.24f, 0.05f, 0.80f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.68f, 0.32f, 0.06f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.85f, 0.42f, 0.08f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.55f, 0.15f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.72f, 0.34f, 0.06f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.46f, 0.10f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.48f, 0.22f, 0.05f, 0.90f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.66f, 0.30f, 0.06f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.82f, 0.40f, 0.08f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.45f, 0.20f, 0.05f, 0.80f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.60f, 0.28f, 0.06f, 0.90f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.75f, 0.36f, 0.08f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.40f, 0.20f, 0.06f, 0.70f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.65f, 0.30f, 0.06f, 0.80f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.85f, 0.40f, 0.08f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.55f, 0.24f, 0.05f, 0.40f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.70f, 0.32f, 0.06f, 0.70f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.88f, 0.44f, 0.08f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.09f, 0.03f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.58f, 0.26f, 0.05f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.42f, 0.18f, 0.04f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.05f, 0.02f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.28f, 0.13f, 0.03f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.42f, 0.08f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.05f, 0.03f, 0.02f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.90f, 0.52f, 0.12f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.68f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.78f, 0.40f, 0.08f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.58f, 0.15f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.15f, 0.08f, 0.03f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.45f, 0.22f, 0.05f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.13f, 0.03f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.95f, 0.55f, 0.10f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.90f, 0.50f, 0.12f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.90f, 0.80f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.04f, 0.03f, 0.02f, 0.65f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.03f, 0.02f, 0.01f, 0.70f);

        style.WindowRounding = 6.0f;
        style.ChildRounding = 5.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 5.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 5.0f;
        style.ScrollbarRounding = 5.0f;
        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 5.0f);
        style.ScrollbarSize = 11.0f;
        style.GrabMinSize = 9.0f;
    }

    static void SetArcticSlateTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.96f, 0.97f, 0.99f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.99f, 1.00f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.72f, 0.76f, 0.84f, 0.80f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_Text] = ImVec4(0.10f, 0.12f, 0.18f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.58f, 0.65f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.31f, 0.28f, 0.90f, 0.25f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.88f, 0.90f, 0.94f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.80f, 0.82f, 0.92f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.70f, 0.72f, 0.90f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.88f, 0.90f, 0.95f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.76f, 0.78f, 0.96f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.90f, 0.92f, 0.96f, 0.75f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.90f, 0.92f, 0.96f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.92f, 0.94f, 0.97f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.55f, 0.75f, 0.80f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.38f, 0.85f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.28f, 0.90f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.30f, 0.27f, 0.90f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.38f, 0.88f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.28f, 0.26f, 0.92f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.38f, 0.35f, 0.88f, 0.90f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.42f, 0.40f, 0.95f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.28f, 0.26f, 0.80f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.38f, 0.36f, 0.88f, 0.70f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.42f, 0.40f, 0.92f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.28f, 0.26f, 0.80f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.72f, 0.74f, 0.82f, 0.80f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.38f, 0.85f, 0.80f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.28f, 0.26f, 0.88f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.45f, 0.42f, 0.88f, 0.35f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.38f, 0.88f, 0.65f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.28f, 0.26f, 0.85f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.86f, 0.88f, 0.94f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.50f, 0.48f, 0.95f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.42f, 0.40f, 0.90f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.90f, 0.92f, 0.96f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.76f, 0.78f, 0.92f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.35f, 0.32f, 0.88f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.90f, 0.92f, 0.96f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.30f, 0.28f, 0.88f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.40f, 0.38f, 0.98f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.32f, 0.30f, 0.85f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.42f, 0.40f, 0.95f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.85f, 0.87f, 0.94f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.62f, 0.65f, 0.78f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.75f, 0.77f, 0.86f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.30f, 0.28f, 0.90f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.32f, 0.30f, 0.88f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.10f, 0.10f, 0.10f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.70f, 0.72f, 0.80f, 0.35f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.60f, 0.62f, 0.70f, 0.50f);

        style.WindowRounding = 8.0f;
        style.ChildRounding = 6.0f;
        style.FrameRounding = 5.0f;
        style.PopupRounding = 6.0f;
        style.GrabRounding = 5.0f;
        style.TabRounding = 6.0f;
        style.ScrollbarRounding = 6.0f;
        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 5.0f);
        style.ScrollbarSize = 11.0f;
        style.GrabMinSize = 9.0f;
    }

    static void SetDeepForestTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.06f, 0.04f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.04f, 0.08f, 0.05f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.09f, 0.06f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.12f, 0.28f, 0.18f, 0.70f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_Text] = ImVec4(0.82f, 0.96f, 0.86f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.32f, 0.48f, 0.36f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.08f, 0.55f, 0.32f, 0.40f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.06f, 0.14f, 0.09f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.08f, 0.24f, 0.15f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.08f, 0.36f, 0.20f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.03f, 0.06f, 0.04f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.05f, 0.22f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.03f, 0.06f, 0.04f, 0.75f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.04f, 0.08f, 0.05f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.05f, 0.03f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.08f, 0.38f, 0.22f, 0.80f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.10f, 0.52f, 0.30f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.12f, 0.68f, 0.40f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.85f, 0.50f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.10f, 0.58f, 0.34f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.14f, 0.75f, 0.45f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.07f, 0.35f, 0.20f, 0.90f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.09f, 0.50f, 0.28f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.12f, 0.65f, 0.36f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.07f, 0.32f, 0.18f, 0.80f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.09f, 0.46f, 0.26f, 0.90f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.11f, 0.60f, 0.34f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.10f, 0.26f, 0.16f, 0.70f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.48f, 0.28f, 0.80f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.12f, 0.64f, 0.36f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.08f, 0.40f, 0.22f, 0.40f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.10f, 0.55f, 0.30f, 0.70f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.14f, 0.70f, 0.40f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.04f, 0.14f, 0.08f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.09f, 0.46f, 0.26f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.07f, 0.32f, 0.18f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.03f, 0.08f, 0.05f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.05f, 0.20f, 0.11f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.12f, 0.65f, 0.36f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.03f, 0.05f, 0.03f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.18f, 0.78f, 0.46f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 0.95f, 0.58f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.14f, 0.65f, 0.38f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.20f, 0.85f, 0.50f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.04f, 0.14f, 0.08f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.10f, 0.30f, 0.18f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.07f, 0.20f, 0.12f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.18f, 0.85f, 0.50f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.16f, 0.78f, 0.46f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.80f, 1.00f, 0.88f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.02f, 0.04f, 0.02f, 0.65f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.01f, 0.03f, 0.02f, 0.70f);

        style.WindowRounding = 6.0f;
        style.ChildRounding = 5.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 5.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 5.0f;
        style.ScrollbarRounding = 5.0f;
        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 5.0f);
        style.ScrollbarSize = 11.0f;
        style.GrabMinSize = 9.0f;
    }

#pragma endregion

    #pragma region ImGuiIcons_to_string
    ////ICONS
    static const std::string ADD_ICON = std::string(ICON_FA_PLUS);
    static const std::string MAGNIFYING_GLASS_ICON = std::string(ICON_FA_MAGNIFYING_GLASS);


    static const std::string FOLDER_ICON            =       std::string(ICON_FA_FOLDER);
    static const std::string MUSIC_ICON             =       std::string(ICON_FA_MUSIC);
    static const std::string XMARK_ICON             =       std::string(ICON_FA_XMARK);

    //Component
    static const std::string CUBE_ICON              =       std::string(ICON_FA_CUBE);
    static const std::string LIGHTBULB_ICON         =       std::string(ICON_FA_LIGHTBULB);
    static const std::string SUN_ICON               =       std::string(ICON_FA_SUN);
    static const std::string CIRCLE_DOT_ICON        =       std::string(ICON_FA_CIRCLE_DOT);
    static const std::string VIDEO_ICON             =       std::string(ICON_FA_VIDEO);
    static const std::string DICE_D6_ICON           =       std::string(ICON_FA_DICE_D6);
    static const std::string VOLUME_HIGH_ICON       =       std::string(ICON_FA_VOLUME_HIGH);

    #pragma endregion
}
