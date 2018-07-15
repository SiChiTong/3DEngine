#include "Time/Timer.h"

using namespace std;

Timer::Timer(double shift = 0.0)
{
		countsPerSecond = 0.0;
		CounterStart = 0;

		frameCount = 0;
		fps = 0;

		frameTime = 0;
		frameTimeOld = 0;
		totalTime = 0;
		seconds = 0;
		this->shift = shift;
}

Timer::~Timer()
{
	
}

void Timer::Update()
{
	CalculateFrameTime();
	totalTime += frameTime;
	frameCount++;
	//If number of seconds since timer started is greater than 1, reset frames counted and timer.
	double time = GetTime();
	
	if(time > 1.0f)
	{
		seconds += time;
		fps = frameCount;
		frameCount = 0;
		Start();
	}	

}

void Timer::Start()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}

double Timer::GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart-CounterStart)/countsPerSecond;
}

void Timer::CalculateFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart-frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if(tickCount < 0.0f)
	{
		tickCount = 0.0;
	}
	frameTime = float(tickCount)/countsPerSecond;
}

double Timer::GetFrameTime()
{
	return this->frameTime;
}

double Timer::GetSeconds()
{
	return seconds + shift;
}

double Timer::GetTotalTime()
{
	return this->totalTime;
}

int Timer::GetFPS()
{
	return fps;
}

