#pragma once


#include "Common/DirectX.h"

class Timer
{
public:
	Timer(double shift);
	~Timer();
	void Update();
	int GetFPS();
	void Start();
	double GetFrameTime();
	double GetTotalTime();
	double GetSeconds();

private:
	
	
	void CalculateFrameTime();
	double GetTime();
	double countsPerSecond;
	__int64 CounterStart;

	int frameCount;
	int fps;

	__int64 frameTimeOld;
	double frameTime;
	double totalTime;
	double seconds;
	double shift;
};
