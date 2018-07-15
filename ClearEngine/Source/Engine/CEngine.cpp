#include "CEngine.h"
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;
using namespace physx;

CEngine::CEngine(GraphicsParams graphicsParams, PhysicsParams physicsParams)
{
	player = Mp3();
	timer = new Timer(0);
	input = new CInput();

	//player.Load(L"../Assets/Music/nucleur.mp3");
	//player.Play();

	InitializeWindow(GetModuleHandle(0), true, L"Window", graphicsParams);

	renderer = new CRenderer(graphicsParams);
	physicsManager = new PhysicsManager(PhysicsManager::DRIVER_DIRECT3D11, renderer->GetDevice(), physicsParams);
	gameAssets = new GameAssets(graphicsParams.modelPath, graphicsParams.texturePath, graphicsParams.animationPath);
	geometryCreator = new GeometryCreator(renderer->GetDevice());
	assetLoader = new AssetLoader(renderer->GetDevice(), renderer->GetDeviceContext(), renderer->GetSwapChain(), gameAssets);
}

CEngine::~CEngine()
{
	delete assetLoader;
	delete geometryCreator;
	delete gameAssets;
	delete input;
	delete timer;
	delete physicsManager;
	delete renderer;
}

//Main part of program. Keeps program running.
int CEngine::Run()
{
	//Message structure for window's message. Holds message information.
	MSG msg;
	//Clears a structure and sets it to NULL. First parameter is address of message and second is the size of message to be deleted.
	ZeroMemory(&msg, sizeof(MSG));

	double frameTime = 0.0f;
	int fps = 0;

	timer->Start();
	//While there is a message.
	while (true)
	{
		//PeekMessage method checks if there was a message.

		//LPMSG lpMsg - This is the pointer to a our message structure
		//HWND hWnd - This is the handle to the window that is sending the message. If this is set to null, it will get messages from any window in the current program.
		//UINT wMsgFilterMin - Specifies the value of the first message in the range of messages to be examined. If wMsgFilterMin and wMsgFilterMax are both set to zero, PeekMessage will examine all messages.
		//UINT wMsgFilterMax - Specifies the value of the last message in the range of messages to be examined.
		//UINT wRemoveMsg - This specifies how the message will be handled. We set to PM_REMOVE so the message will be deleted after we read it.

		//HWND hwnd = GetWindowHandle();
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//Close program if message is a quit message.
			if (msg.message == WM_QUIT)
			{
				break;
			}

			//If our message was a windows message, we translate then dispatch. TranslateMessage() will have windows do some translating like the keyboard's
			//virtual keys to characters. The DispatchMessage() sends the message to our windows procedure, WndProc. After that we return the message.
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//Run game code if no message.
		else
		{
			Update(frameTime, fps);
			Render(frameTime, fps);

			timer->Update();
			frameTime = timer->GetFrameTime();
			fps = timer->GetFPS();
		}
	}
	return msg.wParam;
}

void CEngine::InitializeWindow(HINSTANCE hInstance, int showWnd, LPWSTR wndClassName, GraphicsParams graphicsParams)
{
	typedef struct _WNDCLASS {
		UINT cbSize;
		UINT style;
		WNDPROC lpfnWndProc;
		int cbClsExtra;
		int cbWndExtra;
		HANDLE hInstance;
		HICON hIcon;
		HCURSOR hCursor;
		HBRUSH hbrBackground;
		LPCTSTR lpszMenuName;
		LPCTSTR lpszClassName;
	} WNDCLASS;

	ZeroMemory(&window, sizeof(window));
	//Size of window class.
	window.cbSize = sizeof(WNDCLASSEX);
	//Style of window class. Set so window is redrawn if clicked, moved or resized. Other styles can be used.
	window.style = CS_HREDRAW | CS_VREDRAW;
	//Set pointer to function that processes windows messages.
	window.lpfnWndProc = wndProc;
	//Number of bytes allocated after WNDCLASSEX.
	window.cbClsExtra = NULL;
	//Number of bytes allocated after window instance.
	window.cbWndExtra = NULL;
	//Handle for windows application.
	window.hInstance = hInstance;
	//Load default application icon to appear at top left of window on the title bar. Other icons can be used. This is the default one.
	window.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	//Use arrow cursor on window. This can also be changed.
	window.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//Handle to a brush determining background colour of window.
	window.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	//Name for menu.
	window.lpszMenuName = graphicsParams.appName.c_str();
	//Name of class.
	window.lpszClassName = wndClassName;
	//Specifies icon in task bar.
	window.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	//Try register class and return 1 if fails.
	if (!RegisterClassEx(&window))
	{
		MessageBox(NULL, L"Error registering class",
			L"Error", MB_OK | MB_ICONERROR);
		return;
	}
	//Create window. Putting in extended style/styles, class created or registered earlier, style from old CreateWindow function
	//starting x and y positions of window, height and width of window in pixels, handle to parent window (null here), handle of menu attached to window (null here),
	//instance of current program and parameter to specify if window is used for MDI client but isn't here so null.
	windowHandle = CreateWindowEx(NULL, wndClassName, graphicsParams.appName.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, graphicsParams.screenWidth, graphicsParams.screenHeight, NULL, NULL,
		hInstance, NULL);

	//Check to see if window was successfully created.
	if (!windowHandle)
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	ShowWindow(windowHandle, showWnd);
	UpdateWindow(windowHandle);
}

LRESULT CALLBACK CEngine::wndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Window closed or if escape key down, user prompted to close application and window appears. If the user presses yes, application closes. If no, window closes.
	switch (msg)
	{

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?",
				L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(windowHandle);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	//Takes care of messages received but that have not been taking care of by this program.
	return DefWindowProc(windowHandle,
		msg,
		wParam,
		lParam);
}

PxRigidStatic* CEngine::CreateStaticGroundPlane(PxReal distance)
{
	PxMaterial* material = physicsManager->GetPhysics()->createMaterial(0.5f, 0.5f, 0.0f);
	PxRigidStatic* actor = PxCreatePlane(*GetPhysics(), PxPlane(PxVec3(0, 1, 0), distance), *material);
	return actor;
}

RigidActor* CEngine::CreatePlaneActor(std::wstring name, RigidModel* model, XMFLOAT3 scale, PxReal distance)
{
	if (model->subsets.size() != 1)
	{
		return NULL;
	}
	PxRigidActor* actor = CreateStaticGroundPlane(distance);
	//Set shouldRotate to false because PhysX returns incorrect rotation for the plane.
	RigidActor* entity = new RigidActor(name, model, actor, scale, false);
	return entity;
}

//Ensure to give PxQuat in correct form for the rotation (PxReal for angle in radians and then PxVec3 for normalized axis).
RigidActor* CEngine::CreateTriangleMeshActor(std::wstring name, RigidModel* model, bool dynamic, PxReal scale, PxQuat rotation, PxVec3 position, bool flipNormals)
{
	std::vector<PxVec3> vertices;
	MathUtils::CreatePxVec3Vector(model->vertices, vertices);
	std::vector<PxU32> indices;
	MathUtils::CreatePxU32Vector(model->indices, indices);
	PxTransform transform = CreatePxTransform(rotation, position);

	PxRigidActor* actor = nullptr;
	if (dynamic)
	{
		actor = GetPhysics()->createRigidDynamic(transform);
	}
	else
	{
		actor = GetPhysics()->createRigidStatic(transform);
	}
	PxMaterial* material = GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);
	PxTriangleMesh* mesh = PxToolkit::createTriangleMesh32(*GetPhysics(), *GetCooking(), &vertices[0], vertices.size(), &indices[0], indices.size() / 3, flipNormals);
	PxTriangleMeshGeometry geometry(mesh, PxMeshScale(scale));
	PxShape* shape = actor->createShape(geometry, *material, PxShapeFlag::eSIMULATION_SHAPE);
	RigidActor* entity = new RigidActor(name, model, actor, XMFLOAT3(scale, scale, scale));
	//entity->SetRotation(XMFLOAT4(1.0f, 0, 0, 1.57f));
	return entity;
}

RigidActor* CEngine::CreateBoxActor(std::wstring name, RigidModel* model, PxVec3 scale, PxQuat rotation, PxVec3 position)
{
	if (model->subsets.size() != 1)
	{
		return NULL;
	}
	ModelSubset& subset = model->subsets[0];
	PxReal hx = 0.5f * abs(subset.aabb.max.x - subset.aabb.min.x) * scale.x;
	PxReal hy = 0.5f * abs(subset.aabb.max.y - subset.aabb.min.y) * scale.y;
	PxReal hz = 0.5f * abs(subset.aabb.max.z - subset.aabb.min.z) * scale.z;

	PxTransform transform = CreatePxTransform(rotation, position);
	PxRigidDynamic* actor = GetPhysics()->createRigidDynamic(transform);
	PxMaterial* material = GetPhysics()->createMaterial(0.5f, 0.5f, 0.0f);
	PxShape* shape = actor->createShape(PxBoxGeometry(hx, hy, hz), *material, PxShapeFlag::eSIMULATION_SHAPE);
	RigidActor* entity = new RigidActor(name, model, actor, ConvPxVec3(scale));
	return entity;
}

RigidActor* CEngine::CreateSphereActor(std::wstring name, RigidModel* model, PxReal radius, PxVec3 position)
{
	if (model->subsets.size() != 1)
	{
		return NULL;
	}
	PxTransform transform = CreatePxTransform(PxQuat(0.0f, 0.0f, 0.0f, 1.0f), position);
	PxRigidDynamic* actor = GetPhysics()->createRigidDynamic(transform);
	PxMaterial* material = GetPhysics()->createMaterial(0.5f, 0.5f, 0.0f);
	PxShape* shape = actor->createShape(PxSphereGeometry(radius), *material, PxShapeFlag::eSIMULATION_SHAPE);
	RigidActor* entity = new RigidActor(name, model, actor, XMFLOAT3(radius, radius, radius));
	return entity;
}







