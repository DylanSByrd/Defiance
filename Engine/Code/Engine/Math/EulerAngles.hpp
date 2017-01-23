#pragma once


//-----------------------------------------------------------------------------------------------
class EulerAngles
{
public:
   EulerAngles() {}
   EulerAngles(float inRoll, float inPitch, float inYaw);

   static const EulerAngles ZERO;

   float roll;
   float pitch;
   float yaw;
};




inline EulerAngles::EulerAngles( float inRoll, float inPitch, float inYaw)
: roll(inRoll)
, pitch(inPitch)
, yaw(inYaw)
{}