#include "Game/Map/MapProxy.hpp"
#include "Game/Core/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
std::vector<Vector2i> MapProxy::FindValidAdjacentPositions(const Vector2i& position)
{
   std::vector<Vector2i> validTilePositions;
   for (int directionIndex = 0; directionIndex < NUM_TILE_DIRECTIONS; ++directionIndex)
   {
      Vector2i positionToCheck = m_map->GetTileCoordsInDirection(position, (TileDirection)directionIndex);
      if (!m_map->AreTileCoordsOffMap(positionToCheck))
      {
         const Tile* tileToCheck = m_map->GetTileAtTileCoords(positionToCheck);
         if (tileToCheck->type != STONE_TYPE && !tileToCheck->DoesBlockPathing())
         {
            validTilePositions.push_back(positionToCheck);
         }
      }
   }

   return validTilePositions;
}


//-----------------------------------------------------------------------------------------------
float MapProxy::ComputeLocalAStarG(const Vector2i& start, const Vector2i& destination)
{
   TileDirection dir = m_map->GetDirectionFromSourceToDest(start, destination);

   if (dir == NORTH
      || dir == SOUTH
      || dir == EAST
      || dir == WEST)
   {
      return 1.f;
   }
   else // dir == NORTHEAST || NORTHWEST || SOUTHEAST || SOUTHWEST
   {
      return 1.4f;
   }
}


//-----------------------------------------------------------------------------------------------
float MapProxy::ComputeEstimatedAStarH(const Vector2i& start, const Vector2i& destination)
{
   return (float)start.GetManhattanDistanceToVector(destination);
}