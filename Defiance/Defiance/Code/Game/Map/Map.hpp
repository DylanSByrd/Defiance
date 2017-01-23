#pragma once

#include <string>
#include <vector>
#include "Engine/Math/Vector2i.hpp"
#include "Game/Core/GameCommon.hpp"
#include "Game/Map/Tile.hpp"
#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class Player;
struct Path;
struct RaycastResult;


//-----------------------------------------------------------------------------------------------
class Map
{
public:
   Map();
   Map(const std::vector<Tile>& tiles, const Vector2i& dimensions, const std::string& name);

   bool InitToXMLNode(const XMLNode& node, const std::string& name);
   void UpdateAllTilesToNewType();

   void Render() const;
   void RenderPath(const Path& path) const;
   void RenderRaycastResult(const Vector2f& start, const Vector2f& end, const RaycastResult& result) const;

   Tile* GetTileAtIndex(TileIndex index);
   const Tile* GetTileAtIndex(TileIndex index) const;
   Tile* GetTileAtTileCoords(const TileCoords& coords);
   const Tile* GetTileAtTileCoords(const TileCoords& coords) const;
   TileIndex GetIndexForTileCoords(const TileCoords& location) const;
   TileCoords GetTileCoordsForIndex(TileIndex index) const;
   TileCoords GetTileCoordsInDirection(const TileCoords& location, TileDirection dir, int distance = 1) const;
   const Tile& GetTileInDirection(const TileCoords& location, TileDirection dir, int distance = 1) const;
   int GetNumberOfTilesOfTypeAroundLocationCircular(const TileCoords& location, TileType type, int radius = 1) const;
   int GetNumberOfTilesOfTypeAroundLocationCross(const TileCoords& location, TileType type, int radius = 1) const;
   int GetNumberOfTilesOfTypeAroundIndex(int index, TileType type, int radius = 1) const;
   bool IsTileIndexOffMap(TileIndex index) const;
   bool AreTileCoordsOffMap(const TileCoords& coords) const;
   TileCoords GetRandomEdgeCoords() const;
   void SetTilesInBlockToType(const TileCoords& location, TileType type, int radius = 1);
   void SetTileAtCoordsToType(const TileCoords& location, TileType type);
   TileCoords GetRandomOpenCoords() const;
   void AddFeature(Feature* newFeature, const TileCoords& position);

   bool WriteToXMLNode(XMLNode& parentNode) const;
   std::string GetTilesAsString() const;
   std::string GetVisibilityAsString() const;

   static TileDirection GetOppositeTileDirection(TileDirection originalDirection);
   static TileDirection GetTileDirection90DegreesClockwise(TileDirection originalDirection);
   static TileDirection GetDirectionFromSourceToDest(const TileCoords& source, const TileCoords& destination);
   static Vector2f GetTileCenterFromTileCoords(const TileCoords& coords);

   void ToggleShowUnknownTiles() { m_showAllTiles = !m_showAllTiles; }
   std::vector<Tile>* GetAllTiles() { return &m_tiles; }
   const std::vector<Tile>& GetAllTiles() const { return m_tiles; }
   const Vector2i& GetDimensions() const { return m_dimensions; }
   int GetNumberOfTilesInMap() const { return m_dimensions.x * m_dimensions.y; }


private:
   bool m_showAllTiles;
   std::vector<Tile> m_tiles;
   Vector2i m_dimensions;
   std::string m_name;
};