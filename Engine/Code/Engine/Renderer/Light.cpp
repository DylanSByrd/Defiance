#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Light.hpp"


//-----------------------------------------------------------------------------------------------
STATIC Light Light::MakeGlobalPointLight(const Vector3f& pos, const Rgba& color)
{
   Light newGlobalPointLight;
   newGlobalPointLight.position = pos;
   newGlobalPointLight.color = color;
   newGlobalPointLight.powerAtMaxDistance = 1.f;
   newGlobalPointLight.powerOutsideOuterAngle = 1.f;

   return newGlobalPointLight;
}


//-----------------------------------------------------------------------------------------------
STATIC Light Light::MakeLocalPointLight(const Vector3f& pos, const Rgba& color, float minDistance, float maxDistance)
{
   Light newLocalPointLight;
   newLocalPointLight.position = pos;
   newLocalPointLight.color = color;
   newLocalPointLight.minLightDistance = minDistance;
   newLocalPointLight.maxLightDistance = maxDistance;
   newLocalPointLight.powerOutsideOuterAngle = 1.f;

   return newLocalPointLight;
}


//-----------------------------------------------------------------------------------------------
STATIC Light Light::MakeGlobalSpotLight(const Vector3f& pos, const Vector3f& direction, const Rgba& color, float innerRadiusDegrees, float outerRadiusDegrees)
{
   Light newGlobalSpotLight;
   newGlobalSpotLight.position = pos;
   newGlobalSpotLight.direction = direction;
   newGlobalSpotLight.color = color;
   newGlobalSpotLight.powerAtMaxDistance = 1.f;
   newGlobalSpotLight.innerCosAngle = CosDegrees(innerRadiusDegrees);
   newGlobalSpotLight.outerCosAngle = CosDegrees(outerRadiusDegrees);

   return newGlobalSpotLight;
}


//-----------------------------------------------------------------------------------------------
STATIC Light Light::MakeLocalSpotLight(const Vector3f& pos, const Vector3f& direction, const Rgba& color, float minDistance, float maxDistance, float innerRadiusDegrees, float outerRadius)
{
   Light newLocalSpotLight;
   newLocalSpotLight.position = pos;
   newLocalSpotLight.direction = direction;
   newLocalSpotLight.color = color;
   newLocalSpotLight.minLightDistance = minDistance;
   newLocalSpotLight.maxLightDistance = maxDistance;
   newLocalSpotLight.innerCosAngle = CosDegrees(innerRadiusDegrees);
   newLocalSpotLight.outerCosAngle = CosDegrees(outerRadius);

   return newLocalSpotLight;
}


//-----------------------------------------------------------------------------------------------
STATIC Light Light::MakeDirectionalLight(const Vector3f& direction, const Rgba& color)
{
   Light newDirectionalLight;
   newDirectionalLight.direction = direction.GetNormalized();
   newDirectionalLight.color = color;
   newDirectionalLight.isDirectionalLight = 1;
   newDirectionalLight.powerAtMaxDistance = 1.f;

   return newDirectionalLight;
}