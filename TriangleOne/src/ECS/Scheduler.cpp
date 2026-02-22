#include "ECS/Scheduler.h"

Scheduler::Scheduler(World* world, Window* window) {
	this->world = world;
	this->window = window;

	systemes.push_back(std::make_unique<Time>());
	//systemes.push_back(std::make_unique<RenderModule>());
}

Scheduler::~Scheduler() {};

template<typename SystemeType> void Scheduler::CreateSysteme() {
	std::unique_ptr<SystemeType> systeme = std::make_unique<SystemeType>();
	systemes.push_back(std::move(systeme));
}


void Scheduler::CreateSystemes() {
	//CreateSysteme<Window>();
	//CreateSysteme<Time>();
	//CreateSysteme<RenderSysteme>();

	std::cout << "CreateSystemes done\n";
}

Scheduler* Scheduler::Init() {
	std::cout << "Init Starting" << std::endl;

	for (auto& Systeme : systemes) {
		Systeme->Init();
	}

	std::cout << "Init Done" << std::endl;
	return this;
}

void Scheduler::Update() {

	for (auto& Systeme : systemes) {
		Systeme->Update(*world);
	}
}

void Scheduler::Shutdown() {

	for (auto&  System : systemes) {
		System->Shutdown();
	}
}