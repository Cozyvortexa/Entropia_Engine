#include "ECS/Scheduler.h"

Scheduler::Scheduler(World* world, WindowSystem* window) {
	resourceBuffer = std::make_unique<ResourceBuffer>();
	this->world = world;

	this->window = window;  // Window est géré indépendamment du reste
	world->add_ressource<WindowResource>();
	world->add_ressource<RenderResource>();
	world->add_ressource<TimeResource>();
	world->add_ressource<ActiveCamera>();
	world->add_ressource<InterfaceRessource>();
	world->add_ressource<InputResource>();
	world->add_ressource<InterfaceRessource>();
	FillResourceBuffer();
	window->Init(*world, resourceBuffer.get()); // Systeme a part
}

Scheduler::~Scheduler() {};


void Scheduler::CreateSystemes() {
	systemes.push_back(std::make_unique<TimeSystem>());
	systemes.push_back(std::make_unique<CameraSystem>());
	systemes.push_back(std::make_unique<InputSystem>());
	systemes.push_back(std::make_unique<RenderSystem>());
	systemes.push_back(std::make_unique<LightSystem>());
	systemes.push_back(std::make_unique<InterfaceSystem>());

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
	resourceBuffer->windowResource = world->get_ressource<WindowResource>();
	resourceBuffer->activeCamera = world->get_ressource<ActiveCamera>();
	resourceBuffer->timeResource = world->get_ressource<TimeResource>();
	resourceBuffer->renderResource = world->get_ressource<RenderResource>();
	resourceBuffer->inputResource = world->get_ressource<InputResource>();
	resourceBuffer->interfaceRessource = world->get_ressource<InterfaceRessource>();
}