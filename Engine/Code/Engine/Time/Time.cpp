//-----------------------------------------------------------------------------------------------
// Time.cpp
//	A simple high-precision time utility function for Windows
//	based on code by Squirrel Eiserloh

//-----------------------------------------------------------------------------------------------
#include "Engine/Time/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


//-----------------------------------------------------------------------------------------------
STATIC double Time::s_timeLastFrameBegan = 0.f;
STATIC float Time::s_deltaSeconds = 0.f;


//-----------------------------------------------------------------------------------------------
double InitializeTime( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );
	return( 1.0 / double( countsPerSecond.QuadPart ) );
}


//-----------------------------------------------------------------------------------------------
double Time::GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = double( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}


//-----------------------------------------------------------------------------------------------
void Time::Update()
{
   double timeThisFrameBegan = GetCurrentTimeSeconds();
   s_deltaSeconds = float(timeThisFrameBegan - s_timeLastFrameBegan);
   s_timeLastFrameBegan = timeThisFrameBegan;
}