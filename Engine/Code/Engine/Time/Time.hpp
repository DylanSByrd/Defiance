//-----------------------------------------------------------------------------------------------
// Time.hpp
//	A simple high-precision time utility function for Windows
//	based on code by Squirrel Eiserloh
#pragma once


//-----------------------------------------------------------------------------------------------
class Time
{
public:
   static double GetCurrentTimeSeconds();
   static float GetDeltaSeconds() { return s_deltaSeconds; }
   static void Update();

   static double s_timeLastFrameBegan;
   static float s_deltaSeconds;
};