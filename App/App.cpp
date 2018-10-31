#include "App.h"

using namespace DirectX;

App::App(EngineParams engineParams)
{
	
}

App::~App()
{

}

void App::LoadAssets()
{
	//assetLoader->LoadSkeletalModel(L"soldier/soldier.fbx", L"JG", true, true, XMFLOAT4(1, 0, 0, -1.570796325f));	
}

//void App::DetectInput(double frameTime, int fps)
//{
//	Camera* camera = GetCamera();
//
//	DIMOUSESTATE mouseCurrState;
//
//	BYTE keyboardState[256];
//
//	IDirectInputDevice8* keyboard = input->GetKeyboard();
//	IDirectInputDevice8* mouse = input->GetMouse();
//
//	//Aquire keyboard, mouse and any gamepad from other applications for use.
//	keyboard->Acquire();
//	mouse->Acquire();
//
//	mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
//
//	// Data type that stores all the current states of a controller.
//	XINPUT_STATE cState;
//
//	keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
//	float rotAmount = 2.09f * frameTime;
//	float moveAmount = 5.0f * frameTime;
//	if (input->Check360ControllerStatus(cState))
//	{
//		float leftThumbX = cState.Gamepad.sThumbLX;
//		float leftThumbY = cState.Gamepad.sThumbLY;
//
//		if (cState.Gamepad.wButtons & XINPUT_GAMEPAD_A)
//		{
//			camera->MoveFB(moveAmount);
//		}
//		if (cState.Gamepad.wButtons & XINPUT_GAMEPAD_B)
//		{
//			camera->MoveFB(-moveAmount);
//		}
//		if (leftThumbX > 15000)
//		{
//			camera->SetYaw(rotAmount);
//		}
//		if (leftThumbX < -15000)
//		{
//			camera->SetYaw(-rotAmount);
//		}
//		if (leftThumbY > 15000)
//		{
//			camera->SetPitch(rotAmount);
//		}
//		if (leftThumbY < -15000)
//		{
//			camera->SetPitch(-rotAmount);
//		}
//	}
//
//	if (keyboardState[DIK_W] & 0x80)
//	{
//		camera->SetPitch(rotAmount);
//	}
//	if (keyboardState[DIK_S] & 0x80)
//	{
//		camera->SetPitch(-rotAmount);
//	}
//	if (keyboardState[DIK_D] & 0x80)
//	{
//		camera->SetYaw(rotAmount);
//	}
//	if (keyboardState[DIK_A] & 0x80)
//	{
//		camera->SetYaw(-rotAmount);
//	}
//	if (keyboardState[DIK_UP] & 0x80)
//	{
//		camera->MoveFB(moveAmount);
//	}
//	if (keyboardState[DIK_DOWN] & 0x80)
//	{
//		camera->MoveFB(-moveAmount);
//	}
//
//	if (keyboardState[DIK_T] & 0x80)
//	{
//		GetScene()->setGravity(PxVec3(0.0f, 9.81f, 0.0f));
//	}
//
//	if (keyboardState[DIK_G] & 0x80)
//	{
//		GetScene()->setGravity(PxVec3(0.0f, -9.81f, 0.0f));
//	}
//
//	if (keyboardState[DIK_R] & 0x80)
//	{
//		usePhysics = true;
//	}
//}


