#include "engine.h"

void EntropiaZero::InitEngine() {
	window = std::make_unique<Engine::Systems::WindowSystem>();

	assetStore = std::make_unique<AssetStore>();

	world = std::make_unique<World>(assetStore.get());


	scheduler = std::make_unique<Scheduler>(world.get(), window.get());


	RendererFactory();
	world->renderer = renderer.get();

	//Scheduler
	scheduler->CreateSystemes();
	scheduler->Init();
}

int EntropiaZero::Run() {
	InitEngine();

	while (window->ShouldClose(*world)) {
		scheduler->Update();
	}

	Shutdown();
	return 0;
}


void EntropiaZero::Shutdown() {
	scheduler->Shutdown();

	std::cout << "EntropiaZero off" << std::endl;
}

void EntropiaZero::RendererFactory() {
	Engine::Render::RendererAPI::API graphicAPI = Engine::Render::RendererAPI::GetAPI();

	switch (graphicAPI)
	{
	case Engine::Render::RendererAPI::API::None:
		std::cout << "RendererAPI is set on None" << std::endl;
		abort();
		break;
	case Engine::Render::RendererAPI::API::OpenGL:
		renderer = std::make_unique<OpenGL_Renderer>(assetStore.get());
		break;
	case Engine::Render::RendererAPI::API::Vulkan:
		std::cout << "RendererAPI is set on Vulkan" << std::endl;
		abort();
		break;
	}

}