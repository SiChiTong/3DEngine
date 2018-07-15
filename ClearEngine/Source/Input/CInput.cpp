#include "Input/CInput.h"
 
                                          
CInput::CInput()
{
	if(!InitDirectInput())
	{
		MessageBox(0, L"Direct Input Initialization - Failed",
			L"Error", MB_OK);
		PostMessage(GetActiveWindow(), WM_DESTROY, 0, 0);
	}
}

CInput::~CInput()
{
	keyboard->Unacquire();
	mouse->Unacquire();
	directInput->Release();
}

bool CInput::InitDirectInput()
{
	HRESULT hr;
	hr = DirectInput8Create(GetModuleHandle(0),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&directInput,
		NULL); 

	hr = directInput->CreateDevice(GUID_SysKeyboard,
		&keyboard,
		NULL);

	hr = directInput->CreateDevice(GUID_SysMouse,
		&mouse,
		NULL);

	HWND windowHandle = GetActiveWindow();
	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = keyboard->SetCooperativeLevel(windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = mouse->SetDataFormat(&c_dfDIMouse);
	hr = mouse->SetCooperativeLevel(windowHandle, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

IDirectInputDevice8* CInput::GetKeyboard()
{
	return keyboard;
}

IDirectInputDevice8* CInput::GetMouse()
{
	return mouse;
}

bool CInput::Check360ControllerStatus(XINPUT_STATE& cState)
{
    DWORD dwResult;   // Used to store if a controller is connected
            
	ZeroMemory( &cState, sizeof(XINPUT_STATE) ); // Zeros out the states of
												// the controller.
 
	// Get all the current states of controller 1
	dwResult = XInputGetState( 0, &cState);
 
	if( dwResult == ERROR_SUCCESS )
	{
		return true;
	}
	else
	{
		return false;
	}
}