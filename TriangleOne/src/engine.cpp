#include "engine.h"

Engine* Engine::instance = nullptr;

Engine::Engine() {
	if (instance != nullptr) { throw std::runtime_error("Engine instance already exists!"); }

	instance = this;
}

template<typename ModuleType> ModuleType* Engine::CreateModule() {
	ModuleType* module = new ModuleType();
	modules.push_back(module);
	return module;
}

Engine::~Engine() {};

int Engine::Run() {

	CreateModules();
	Init();

	while (true) {
		Update();
		Render();

	}
}

void Engine::CreateModules() {
	window = CreateModule<Window>();
	CreateModule<RenderModule>();

	std::cout << "CreateModules done\n";
}

Engine* Engine::Init() {
	std::cout << "Init Starting" << std::endl;

	for (Module* module : modules) {
		module->Init();
	}

	std::cout << "Init Done" << std::endl;
	return this;
}

void Engine::Update() {

	for (Module* module : modules) {
		module->Update();
	}
}

void Engine::Render() {

	for (Module* module : modules) {
		module->Render();
	}
}

void Engine::Shutdown() {

	for (Module* module : modules) {
		module->Shutdown();
		delete module;
	}
}