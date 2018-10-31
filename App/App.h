#pragma once

#include <EngineAPI/CApp.h>

class App
{
public:
	App(EngineParams engineParams);
	~App();
	void Update(double frameTime, int fps);

private:
	void LoadAssets();
	void CreateActors();
	void DetectInput(double frameTime, int fps);
	bool usePhysics;
};
