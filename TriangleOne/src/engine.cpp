#include "engine.h"

Engine* Engine::instance = nullptr;

Engine::Engine() {
	if (instance != nullptr) { throw std::runtime_error("Engine instance already exists!"); }

	instance = this;
}

Engine::~Engine() {};

void Engine::InitEngine() {
	window = std::make_unique<WindowSystem>();

	modelStore = std::make_unique<ModelStore>();

	world = std::make_unique<World>(modelStore.get());
	scheduler = std::make_unique<Scheduler>(world.get(), window.get());

	//Scheduler
	scheduler->CreateSystemes();
	scheduler->Init();
}

int Engine::Run() {
	InitEngine();

	while (window->ShouldClose(*world)) {
		scheduler->Update();
	}

	//Entity entity = 10;
	//Position position;
	//position.x = 44;
	//world->add_component(entity, position);
	//View view = world->view<Position>();
	//view.each([this](int entity, Position& pos) {
	//	std::cout << pos.x << " enter here " << std::endl;
	//});


	//while (window->ShouldClose()) {
	//	Update();
	//	Render();
	//}
	Shutdown();
	return 0;
}


void Engine::Shutdown() {
	scheduler->Shutdown();

	std::cout << "Engine off" << std::endl;
}