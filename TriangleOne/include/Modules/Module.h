#pragma once

class Module {
public:
	virtual void Init() {}
	virtual void Update() {}
	virtual void Render() {}
	virtual void Shutdown() {}
};
