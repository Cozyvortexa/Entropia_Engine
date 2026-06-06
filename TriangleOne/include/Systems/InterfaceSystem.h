#pragma once
#define NOMINMAX
#include "ECS/System.h"

#include <filesystem>
#include <stack>

#include "Utilities/ImGui/imgui.h"
#include "Utilities/ImGui/imgui_internal.h"
#include "Utilities/ImGui/backends/imgui_impl_glfw.h"
#include "Utilities/ImGui/backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <iostream>

#include "ECS/World.h"
#include "ECS/Components/Component.h"

#include "Utilities/ImGui/imgui_stdlib.h"
#include "Systems/RenderSystem.h"

#include "Utilities/Font_awesome.h"
#include "Utilities/EditorComponent.h"

#include <windows.h>  // TEMP

namespace Engine::Systems {
	class InterfaceSystem : public System {
		void Init(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Update(World& world, const Engine::Resource::ResourceBuffer* ressourceBuffer) override;
		void Shutdown(World& world) override;

		std::unique_ptr<Resource::Node> BuildTree(const std::filesystem::path& rootPath, Engine::Resource::Node* parent = nullptr);

	private:
		Resource::FileType GetType(const std::filesystem::path& path);
		void Display_ArboMenu(Resource::InterfaceRessource* interfaceData);
		void inline Arbo_RefreshButton(Resource::InterfaceRessource* interfaceData);

		Entity CreateNewEntity(World* world);
		void Add_Entity_Button(World* world, Engine::Resource::RenderResource* renderData, Engine::Resource::AudioResource* audioData, std::string label, int entity);
		void Display_Hierarchy_Menu(World* world, const Resource::ResourceBuffer* resourceBuffer);


		void Display_Inspecteur_Menu(World* world, const Engine::Resource::ResourceBuffer* resourceBuffer);
	};
}