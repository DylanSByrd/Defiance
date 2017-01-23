#include "Game/FieldOfView/FieldOfViewAdvanced.hpp"
#include "Game/Entities/Agents/Agent.hpp"


//-----------------------------------------------------------------------------------------------
void FieldOfViewAdvanced::CalculateFieldOfViewForAgent(Agent* agent, int viewDistance, Map* map, bool isPlayer)
{
   TileCoords agentPos = agent->GetPosition();

   TileCoords topLeftTile(0, 0);
   TileCoords botRightTile(map->GetDimensions() - TileCoords(1,1));

   for (int yIndex = topLeftTile.y; yIndex <= botRightTile.y; ++yIndex)
   {
      for (int xIndex = topLeftTile.x; xIndex <= botRightTile.x; ++xIndex)
      {
         TileCoords coordsToCheck(xIndex, yIndex);
         Tile* tileToCheck = map->GetTileAtTileCoords(coordsToCheck);
         
         if (agentPos.GetManhattanDistanceToVector(coordsToCheck) > viewDistance
            && isPlayer)
         {
            tileToCheck->isVisible = false;
         }
         
         if (coordsToCheck == agentPos
            && isPlayer)
         {
            tileToCheck->isVisible = true;
            tileToCheck->isKnown = true;
            continue;
         }

         bool isTileVisible = RaycastFromTileCornersToTileCorners(agentPos, coordsToCheck, map);
         if (!isTileVisible)
         {
            if (isPlayer)
            {
               tileToCheck->isVisible = false;
            }

            continue;
         }

         float distanceToTile = Vector2i::GetDistanceBetween(agentPos, coordsToCheck);
         if (tileToCheck->IsOccupiedByAgent())
         {
            agent->AddVisibleAgent(distanceToTile, tileToCheck->occupyingAgent);
         }

         if (tileToCheck->HasItems())
         {
            agent->AddVisibleItems(distanceToTile, tileToCheck->GetItems());
         }

         if (tileToCheck->HasAFeature())
         {
            agent->AddVisibleFeature(distanceToTile, tileToCheck->occupyingFeature);
         }

         if (isPlayer)
         {
            tileToCheck->isVisible = true;
            tileToCheck->isKnown = true;
         }
         continue;
      }
   }
}


//-----------------------------------------------------------------------------------------------
bool FieldOfViewAdvanced::RaycastFromTileCornersToTileCorners(const TileCoords& startTile, const TileCoords& endTile, Map* map) const
{
   bool isOneCornerVisible = true;

   Vector2f botLeftEndCorner = endTile;
   Vector2f botRightEndCorner = botLeftEndCorner + Vector2f(1.f, 0.f);
   Vector2f topRightEndCorner = botLeftEndCorner + Vector2f(1.f, 1.f);
   Vector2f topLeftEndCorner = botLeftEndCorner + Vector2f(0.f, 1.f);

   if (IsCornerVisible(startTile, endTile, botLeftEndCorner, map))
   {
      return isOneCornerVisible;
   }

   if (IsCornerVisible(startTile, endTile, botRightEndCorner, map))
   {
      return isOneCornerVisible;
   }

   if (IsCornerVisible(startTile, endTile, topRightEndCorner, map))
   {
      return isOneCornerVisible;
   }
   
   if (IsCornerVisible(startTile, endTile, topLeftEndCorner, map))
   {
      return isOneCornerVisible;
   }

   return !isOneCornerVisible;
}


//-----------------------------------------------------------------------------------------------
bool FieldOfViewAdvanced::IsCornerVisible(const TileCoords& startTile, const TileCoords& endTile, const Vector2f& cornerPos, Map* map) const
{
   bool isCornerVisible = true;

   Vector2f botLeftStartCorner = startTile;
   Vector2f botRightStartCorner = botLeftStartCorner + Vector2f(1.f, 0.f);
   Vector2f topRightStartCorner = botLeftStartCorner + Vector2f(1.f, 1.f);
   Vector2f topLeftStartCorner = botLeftStartCorner + Vector2f(0.f, 1.f);

   RaycastResult castResult;
   Raycast(botLeftStartCorner, cornerPos, &castResult, map);
   if (!castResult.didImpact || castResult.impactCoords == endTile)
   {
      return isCornerVisible;
   }

   Raycast(botRightStartCorner, cornerPos, &castResult, map);
   if (!castResult.didImpact || castResult.impactCoords == endTile)
   {
      return isCornerVisible;
   }

   Raycast(topRightStartCorner, cornerPos, &castResult, map);
   if (!castResult.didImpact || castResult.impactCoords == endTile)
   {
      return isCornerVisible;
   }

   Raycast(topLeftStartCorner, cornerPos, &castResult, map);
   if (!castResult.didImpact || castResult.impactCoords == endTile)
   {
      return isCornerVisible;
   }

   return !isCornerVisible;
}

