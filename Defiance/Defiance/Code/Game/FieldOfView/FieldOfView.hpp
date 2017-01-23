#pragma once

#include "Engine/Math/Vector2f.hpp"
#include "Game/Map/MapProxy.hpp"


//-----------------------------------------------------------------------------------------------
class Agent;
class Map;


//-----------------------------------------------------------------------------------------------
struct RaycastResult
{
   RaycastResult() {}

   bool didImpact;
   Vector2f impactPos;
   Vector2i impactCoords;
   Vector2f impactNorm;
};


//-----------------------------------------------------------------------------------------------
class FieldOfView
{
public:
   virtual void CalculateFieldOfViewForAgent(Agent* agent, int viewDistance, Map* map, bool isPlayer) = 0;
   static bool Raycast(const Vector2f& startPos, const Vector2f& endPos, RaycastResult* outResult, Map* map);
};