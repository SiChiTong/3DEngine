#pragma once

#include <Math/Cmath.h>
#include "EngineAPI/APITypes.h"

class CApp 
{
public:
	virtual ~CApp();

protected:
	CApp(EngineParams engineParams);
	void Update(double frameTime, int fps);
};

