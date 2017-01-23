#include "Engine/Core/EngineCommon.hpp"
#include "Game/FieldOfView/FieldOfView.hpp"


//-----------------------------------------------------------------------------------------------
STATIC bool FieldOfView::Raycast(const Vector2f& startPos, const Vector2f& endPos, RaycastResult* outResult, Map* map)
{
   Vector2f rayDisplacement = endPos - startPos;
   Vector2i startCoords(startPos);
   Vector2i rayCoords(startPos);

   const Tile* startingTile = map->GetTileAtTileCoords(rayCoords);
   if (startingTile->type == STONE_TYPE || startingTile->DoesBlockLineOfSight())
   {
      outResult->didImpact = true;
      outResult->impactPos = startPos;
      outResult->impactCoords = rayCoords;
      outResult->impactNorm = Vector2f::ZERO;
      return true;
   }

   float tDeltaX = abs(1.0f / rayDisplacement.x);
   int tileStepX = 1;
   if (rayDisplacement.x < 0)
   {
      tileStepX = -1;
   }
   int offsetToLeadingEdgeX = (tileStepX + 1) / 2;
   float firstVerticalIntersectionX = (float)(startCoords.x + offsetToLeadingEdgeX);
   float tOfNextXCrossing = abs(firstVerticalIntersectionX - startPos.x) * tDeltaX;

   float tDeltaY = abs(1.0f / rayDisplacement.y);
   int tileStepY = 1;
   if (rayDisplacement.y < 0)
   {
      tileStepY = -1;
   }
   int offsetToLeadingEdgeY = (tileStepY + 1) / 2;
   float firstVerticalIntersectionY = (float)(startCoords.y + offsetToLeadingEdgeY);
   float tOfNextYCrossing = abs(firstVerticalIntersectionY - startPos.y) * tDeltaY;

   bool finished = false;
   while (!finished)
   {
      if (tOfNextXCrossing < tOfNextYCrossing)
      {
         if (tOfNextXCrossing > 1)
         {
            outResult->didImpact = false;
            outResult->impactPos = endPos;
            outResult->impactCoords = endPos;
            outResult->impactNorm = Vector2f((float)-tileStepX, 0.f);
            break;
         }

         rayCoords.x += tileStepX;
         const Tile* currentTile = map->GetTileAtTileCoords(rayCoords);
         if (currentTile->type == STONE_TYPE || currentTile->DoesBlockLineOfSight())
         {
            outResult->didImpact = true;
            outResult->impactPos = startPos + (rayDisplacement * tOfNextXCrossing);
            outResult->impactCoords = rayCoords;
            outResult->impactNorm = Vector2f((float)-tileStepX, 0.f);
            finished = true;
         }
         else
         {
            tOfNextXCrossing += tDeltaX;
         }
      }
      else
      {
         if (tOfNextYCrossing > 1)
         {
            outResult->didImpact = false;
            outResult->impactPos = endPos;
            outResult->impactCoords = endPos;
            outResult->impactNorm = Vector2f(0.f, (float)-tileStepY);
            break;
         }

         rayCoords.y += tileStepY;
         const Tile* currentTile = map->GetTileAtTileCoords(rayCoords);
         if (currentTile->type == STONE_TYPE || currentTile->DoesBlockLineOfSight())
         {
            outResult->didImpact = true;
            outResult->impactPos = startPos + (rayDisplacement * tOfNextYCrossing);
            outResult->impactCoords = rayCoords;
            outResult->impactNorm = Vector2f(0.f, (float)-tileStepY);
            finished = true;
         }
         else
         {
            tOfNextYCrossing += tDeltaY;
         }
      }
   }
   return outResult->didImpact;
}