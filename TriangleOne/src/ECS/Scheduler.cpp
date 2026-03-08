#include "ECS/Scheduler.h"

Scheduler::Scheduler(World* world, WindowSystem* window) {
	resourceBuffer = std::make_unique<ResourceBuffer>();
	this->world = world;

	this->window = window;  // Window est géré indépendamment du reste
	world->add_ressource<WindowResource>();
	window->Init(*world); // Systeme a part

}

Scheduler::~Scheduler() {};


void Scheduler::CreateSystemes() {
	systemes.push_back(std::make_unique<TimeSystem>());
	world->add_ressource<TimeResource>();

	systemes.push_back(std::make_unique<CameraSys>());
	world->add_ressource<ActiveCamera>();

	//systemes.push_back(std::make_unique<RenderModule>());

	std::cout << "CreateSystemes done\n";
}

Scheduler* Scheduler::Init() {
	std::cout << "Init Starting" << std::endl;

	for (auto& Systeme : systemes) {
		Systeme->Init(*world);
	}

	std::cout << "Init Done" << std::endl;
	return this;
}

void Scheduler::Update() {
	FillResourceBuffer();
	window->Update(*world, resourceBuffer.get());


	for (auto& Systeme : systemes) {
		Systeme->Update(*world, resourceBuffer.get());
	}
}

void Scheduler::Shutdown() {
	window->Shutdown(*world);


	for (auto&  System : systemes) {
		System->Shutdown(*world);
	}
}

void Scheduler::FillResourceBuffer() {
	resourceBuffer->windowResource = world->get_ressource<WindowResource>();
	resourceBuffer->activeCamera = world->get_ressource<ActiveCamera>();
	resourceBuffer->timeResource = world->get_ressource<TimeResource>();
	resourceBuffer->renderResource = world->get_ressource<RenderResource>();
}