#include "Game/FieldOfView/FieldOfViewBasic.hpp"
#include "Game/Entities/Agents/Agent.hpp"


//-----------------------------------------------------------------------------------------------
void FieldOfViewBasic::CalculateFieldOfViewForAgent(Agent* agent, int viewDistance, Map* map, bool isPlayer)
{
   TileCoords agentPos = agent->GetPosition();

   TileCoords topLeftTile(agentPos.x - viewDistance, agentPos.y - viewDistance);
   TileCoords botRightTile(agentPos.x + viewDistance, agentPos.y + viewDistance);

   std::vector<Tile>* tiles = map->GetAllTiles();
   for (int yIndex = topLeftTile.y; yIndex <= botRightTile.y; ++yIndex)
   {
      for (int xIndex = topLeftTile.x; xIndex <= botRightTile.x; ++xIndex)
      {
         TileCoords tileToCheck(xIndex, yIndex);

         if (map->AreTileCoordsOffMap(tileToCheck))
         {
            continue;
         }

         TileIndex indexToCheck = map->GetIndexForTileCoords(tileToCheck);
         
         if (tileToCheck == agentPos)
         {
            if (isPlayer)
            {
               (*tiles)[indexToCheck].isVisible = true;
               (*tiles)[indexToCheck].isKnown = true;
            }

            continue;
         }

         RaycastResult result;
         Raycast(Map::GetTileCenterFromTileCoords(agentPos), Map::GetTileCenterFromTileCoords(tileToCheck), &result, map);

         if (!result.didImpact || result.impactCoords == tileToCheck)
         {
            if (isPlayer)
            {
               (*tiles)[indexToCheck].isVisible = true;
               (*tiles)[indexToCheck].isKnown = true;
            }
         }
         else
         {
            if (isPlayer)
            {
               (*tiles)[indexToCheck].isVisible = false;
            }
         }
      }
   }
}