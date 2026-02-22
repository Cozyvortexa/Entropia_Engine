#pragma once

class World;

class System {
public:
	virtual void Init() {};
	virtual void Update(World& world) {};
	virtual void Shutdown() {};
};
