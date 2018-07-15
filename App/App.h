#ifndef App_H
#define App_H

#pragma comment(lib,"ClearEngine.lib")

#include <Math/Cmath.h>
#include "AssetLoadingData.h"

class App : public CEngine
{
public:
	App(GraphicsParams graphicsParams, PhysicsParams physicsParams);
	~App();

protected:
	void Update(double frameTime, int fps);
	void Render(double frameTime, int fps);

private:
	void LoadAssets();
	void CreateActors();
	void DetectInput(double frameTime, int fps);
	RigidActor* level;
	SkeletalActor* vil;
	bool usePhysics;
};

#endif