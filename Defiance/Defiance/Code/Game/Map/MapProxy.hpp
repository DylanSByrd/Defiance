#pragma once

#include <vector>
#include "Engine/Math/Vector2i.hpp"
#include "Game/Map/Map.hpp"


//-----------------------------------------------------------------------------------------------
class MapProxy
{
public:
   MapProxy(Map* map) : m_map(map) {}
   ~MapProxy() {}

   // #TODO - expand to take some sort of tile validity parameters
   std::vector<Vector2i> FindValidAdjacentPositions(const Vector2i& position);
   float ComputeLocalAStarG(const Vector2i& start, const Vector2i& destination);
   float ComputeEstimatedAStarH(const Vector2i& start, const Vector2i& destination);

private:
   Map* m_map;
};