#ifndef CInput_H
#define CInput_H

#include "Common/DirectX.h"
#include "Graphics/Camera/Camera.h"
   

class CInput
{
public:
	CInput();
	~CInput();
	bool Check360ControllerStatus(XINPUT_STATE& cState);
	IDirectInputDevice8* GetKeyboard();
	IDirectInputDevice8* GetMouse();
  
private:
	bool InitDirectInput();
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;
	LPDIRECTINPUT8 directInput;
};
#endif