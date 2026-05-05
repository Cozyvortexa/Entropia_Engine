#include "ECS/Scheduler.h"

Scheduler::Scheduler(World* world, Engine::Systems::WindowSystem* window) {
	resourceBuffer = std::make_unique<Engine::Resource::ResourceBuffer>();
	this->world = world;

	this->window = window;  // Window est géré indépendamment du reste
	world->add_ressource<Engine::Resource::WindowResource>();
	world->add_ressource<Engine::Resource::RenderResource>();
	world->add_ressource<Engine::Resource::TimeResource>();
	world->add_ressource<Engine::Resource::ActiveCamera>();
	world->add_ressource<Engine::Resource::InterfaceRessource>();
	world->add_ressource<Engine::Resource::InputResource>();
	world->add_ressource<Engine::Resource::InterfaceRessource>();
	FillResourceBuffer();
	window->Init(*world, resourceBuffer.get()); // Systeme a part
}

Scheduler::~Scheduler() {};


void Scheduler::CreateSystemes() {
	systemes.push_back(std::make_unique<Engine::Systems::TimeSystem>());
	systemes.push_back(std::make_unique<Engine::Systems::CameraSystem>());
	systemes.push_back(std::make_unique<Engine::Systems::InputSystem>());
	systemes.push_back(std::make_unique<Engine::Systems::RenderSystem>());
	systemes.push_back(std::make_unique<Engine::Systems::LightSystem>());
	systemes.push_back(std::make_unique<Engine::Systems::InterfaceSystem>());

	std::cout << "CreateSystemes done" << std::endl;;
}

Scheduler* Scheduler::Init() {
	std::cout << "Init Starting" << std::endl;
	FillResourceBuffer();

	for (auto& systeme : systemes) {
		systeme->Init(*world, resourceBuffer.get());
	}

	std::cout << "Init Done" << std::endl;
	return this;
}

void Scheduler::Update() {
	FillResourceBuffer();
	window->Update(*world, resourceBuffer.get());


	for (auto& systeme : systemes) {
		systeme->Update(*world, resourceBuffer.get());
	}
}

void Scheduler::Shutdown() {
	std::cout << "Shutdown Start" << std::endl;
	window->Shutdown(*world);


	for (auto&  system : systemes) {
		system->Shutdown(*world);
	}
	std::cout << "Shutdown Done" << std::endl;
}

void Scheduler::FillResourceBuffer() {
	resourceBuffer->windowResource = world->get_ressource<Engine::Resource::WindowResource>();
	resourceBuffer->activeCamera = world->get_ressource<Engine::Resource::ActiveCamera>();
	resourceBuffer->timeResource = world->get_ressource<Engine::Resource::TimeResource>();
	resourceBuffer->renderResource = world->get_ressource<Engine::Resource::RenderResource>();
	resourceBuffer->inputResource = world->get_ressource<Engine::Resource::InputResource>();
	resourceBuffer->interfaceRessource = world->get_ressource<Engine::Resource::InterfaceRessource>();
}