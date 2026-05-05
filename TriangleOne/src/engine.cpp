#include "engine.h"

EntropiaZero* EntropiaZero::instance = nullptr;

EntropiaZero::EntropiaZero() {
	if (instance != nullptr) { throw std::runtime_error("EntropiaZero instance already exists!"); }

	instance = this;
}

EntropiaZero::~EntropiaZero() {};

void EntropiaZero::InitEngine() {
	window = std::make_unique<Engine::Systems::WindowSystem>();

	assetStore = std::make_unique<AssetStore>();

	world = std::make_unique<World>(assetStore.get());
	scheduler = std::make_unique<Scheduler>(world.get(), window.get());

	renderer = std::make_unique<Renderer>(assetStore.get());
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