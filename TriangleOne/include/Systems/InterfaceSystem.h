#pragma once
#include "ECS/System.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace Engine::Systems {
	class InterfaceSystem : public System {
		void Init(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Update(World& world, const Engine::Resource::ResourceBuffer* ressourceBuffer) override;
		void Shutdown(World& world) override;
	};
}