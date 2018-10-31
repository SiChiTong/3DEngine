#include "App.h"
#include <windows.h>

#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

int main(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int nCmdShow)
{
	GraphicsParams graphicsParams;
	graphicsParams.animationPath = L"../Assets/Animations/";
	graphicsParams.appName = L"Demo";
	graphicsParams.modelPath = L"../Assets/Models/";
	graphicsParams.screenHeight = 1080;
	graphicsParams.screenWidth = 1920;
	graphicsParams.texturePath = L"../Assets/Textures/";
	graphicsParams.vSyncEnabled = false;
	graphicsParams.windowed = true;

	App* app = new App(graphicsParams, PhysicsParams());
	app->Run();
	delete app;
	return 0;
}

