#pragma once
#include <iostream>

#include "ECS/System.h"

#include "ECS/Components/AudioStruct.h"
#include "ECS/Components/Component.h"

#include <ECS/World.h>


namespace Engine::Systems {
	class AudioSystem : public System {
	public:

		void Init(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Update(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Shutdown(World& world) override;

		//void test3D();
	private:
		void UpdatePositionAudio(World* world, Resource::AudioResource* audioData);
	};
}