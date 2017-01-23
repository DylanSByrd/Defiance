#pragma once

#include "Engine/Math/Vector3f.hpp"
#include "Engine/Renderer/Rgba.hpp"


// #TODO - probably should have make funcs return ptrs
//-----------------------------------------------------------------------------------------------
struct Light
{
   static Light MakeGlobalPointLight(const Vector3f& pos, const Rgba& color);
   static Light MakeLocalPointLight(const Vector3f& pos, const Rgba& color, float minDistance, float maxDistance);
   static Light MakeGlobalSpotLight(const Vector3f& pos, const Vector3f& direction, const Rgba& color, float innerRadiusDegrees, float outerRadiusDegrees);
   static Light MakeLocalSpotLight(const Vector3f& pos, const Vector3f& direction, const Rgba& color, float minDistance, float maxDistance, float innerRadiusDegrees, float outerRadiusDegrees);
   static Light MakeDirectionalLight(const Vector3f& direction, const Rgba& color);

   Vector3f position = Vector3f::ZERO;
   Vector3f direction = Vector3f::ZERO;
   Rgba     color = Rgba::BLACK;
   int     isDirectionalLight = 0; //treat as bool
   float    minLightDistance = 0.f;
   float    maxLightDistance = 1.f;
   float    powerAtMinDistance = 1.f;
   float    powerAtMaxDistance = 0.f;
   float    innerCosAngle = 1.f;
   float    outerCosAngle = -1.f;
   float    powerInsideInnerAngle = 1.f;
   float    powerOutsideOuterAngle = 0.f;
};