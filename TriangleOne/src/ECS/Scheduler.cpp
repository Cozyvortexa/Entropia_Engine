#include "ECS/Scheduler.h"

Scheduler::Scheduler(World* world, Window* window) {
	this->world = world;

	this->window = window;  // Window est géré indépendamment du reste
	world->add_ressource<WindowRessource>();
	window->Init(*world); // Systeme a part

}

Scheduler::~Scheduler() {};


void Scheduler::CreateSystemes() {
	systemes.push_back(std::make_unique<Time>());
	world->add_ressource<TimeRessource>();

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

	window->Update(*world);

	for (auto& Systeme : systemes) {
		Systeme->Update(*world);
	}
}

void Scheduler::Shutdown() {
	window->Shutdown(*world);


	for (auto&  System : systemes) {
		System->Shutdown(*world);
	}
}