#pragma once

#include "Game/FieldOfView/FieldOfView.hpp"


//-----------------------------------------------------------------------------------------------
class FieldOfViewAdvanced
   : public FieldOfView
{
public:
   virtual void CalculateFieldOfViewForAgent(Agent* agent, int viewDistance, Map* map, bool isPlayer) override;
   bool RaycastFromTileCornersToTileCorners(const TileCoords& startTile, const TileCoords& endTile, Map* map) const;
   bool IsCornerVisible(const TileCoords& startTile, const TileCoords& endTile, const Vector2f& cornerPos, Map* map) const;
};