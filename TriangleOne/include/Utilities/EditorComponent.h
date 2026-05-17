#pragma once
#include <string>
#include <vector>
#include "Utilities/Font_awesome.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ECS/Components/Component.h"

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

    template<typename T>
    inline void DrawWidget(const char* label, T& value) {};  // Fallback

    template<>
    inline void DrawWidget<int>(const char* label, int& value) {
        ImGui::InputInt(label, &value);
    }

    template<>
    inline void DrawWidget<float>(const char* label, float& value) {
        ImGui::InputFloat(label, &value);
    }

    template<>
    inline void DrawWidget<bool>(const char* label, bool& value) {
        ImGui::Checkbox(label, &value);
    }

    template<>
    inline void DrawWidget<glm::vec2>(const char* label, glm::vec2& value) {
        ImGui::InputFloat2(label, &value.x);
    }

    template<>
    inline void DrawWidget<glm::vec3>(const char* label, glm::vec3& value) {
        ImGui::InputFloat3(label, &value.x);
    }

    template<>
    inline void DrawWidget<glm::vec4>(const char* label, glm::vec4& value) {
        ImGui::InputFloat4(label, &value.x);
    }


    template<typename T>
    inline void DrawComponentUI(T& component)
    {
        component.Reflect([&](const char* name, auto& value)
            {
                Engine::Editor::DrawWidget(name, value);
            });
    }

    template<typename T>
    bool DrawComponentSection(World* world, Entity entity, const char* label)
    {
        T* component = world->get_component<T>(entity);

        if (!component)
            return false;

        ImGui::PushID(label);

        bool open = ImGui::CollapsingHeader(label);

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Component"))
            {
                world->remove_component<T>(entity);

                ImGui::EndPopup();
                ImGui::PopID();
                return false;
            }

            ImGui::EndPopup();
        }

        if (open)
        {
            Editor::DrawComponentUI(*component);
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
