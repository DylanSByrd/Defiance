#include <sstream>
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Debug/DebugRenderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Generators/Generator.hpp"
#include "Game/Core/GameCommon.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Map/Tile.hpp"
#include "Game/Entities/Agents/Player.hpp"
#include "Game/Entities/Features/Feature.hpp"
#include "Game/Pathfinding/Pathfinder.hpp"
#include "Game/FieldOfView/FieldOfView.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map()
   : m_showAllTiles(false)
   , m_dimensions(Vector2i::ZERO)
   , m_name("")
{}


//-----------------------------------------------------------------------------------------------
Map::Map(const std::vector<Tile>& tiles, const Vector2i& dimenisons, const std::string& name)
   : m_showAllTiles(false)
   , m_tiles(tiles)
   , m_dimensions(dimenisons)
   , m_name(name)
{}


//-----------------------------------------------------------------------------------------------
bool Map::InitToXMLNode(const XMLNode& node, const std::string& name)
{
   bool initSuccessful = true;

   Vector2i dimensions = ReadXMLAttribute(node, "mapSize", Vector2i::ZERO);
   m_dimensions = dimensions;

   XMLNode tileDataNode = node.getChildNode("TileData");
   
   ASSERT_OR_DIE(!tileDataNode.isEmpty(), "ERROR: TileData field empty!");

   std::string rawTileData = tileDataNode.getText();
   std::stringstream tileDataStream(rawTileData);
   
   std::string tileRow;
   std::vector<std::string> tileRows;
   while (tileDataStream >> tileRow)
   {
      tileRows.push_back(tileRow);
   }

   m_dimensions.y = tileRows.size();

   // Longest string will be the minimum width
   int maxLength = 0;
   for (const std::string& row : tileRows)
   {
      int currentRowSize = row.size();
      if (currentRowSize > maxLength)
      {
         maxLength = currentRowSize;
      }
   }

   m_dimensions.x = maxLength;

   // Make space for map, then init tiles
   m_tiles.clear();
   m_tiles.reserve(m_dimensions.x * m_dimensions.y);

   for (int rowIndex = tileRows.size() - 1; rowIndex >= 0; --rowIndex)
   {
      const std::string& row(tileRows[rowIndex]);
      for (int charIndex = 0; charIndex < m_dimensions.x; ++charIndex)
      {
         // If characters are missing, make stone tiles
         if (charIndex >= (int)row.size())
         {
            m_tiles.push_back(Tile(STONE_TYPE));
            continue;
         }

         // #TODO - update this with legend!
         if (row[charIndex] == '#')
         {
            m_tiles.push_back(Tile(STONE_TYPE));
         }
         else if (row[charIndex] == '~')
         {
            m_tiles.push_back(Tile(WATER_TYPE));
         }
         else // char = .
         {
            m_tiles.push_back(Tile(AIR_TYPE));
         }
      }
   }
   Generator::FinalizeMap(this);

   // Check for visibility data
   XMLNode visibilityNode = node.getChildNode("VisibilityData");
   if (!visibilityNode.isEmpty())
   {
      std::string visibilityString = visibilityNode.getText();
      int stringIndex = 0;
      for (int yIndex = 0; yIndex < m_dimensions.y; ++yIndex)
      {
         for (int xIndex = 0; xIndex < m_dimensions.x; ++xIndex)
         {
            TileIndex tileIndex = GetIndexForTileCoords(TileCoords(xIndex, yIndex));
            Tile* tile = GetTileAtIndex(tileIndex);

            if (visibilityString[stringIndex] == '#')
            {
               tile->isKnown = true;
            }
            ++stringIndex;
         }

         // eat the newline
         ++stringIndex;
      }
   }

   // Finally, update name to reflect the data file
   m_name = name;


   return initSuccessful;
}


//-----------------------------------------------------------------------------------------------
void Map::UpdateAllTilesToNewType()
{
   for (Tile& tile : m_tiles)
   {
      tile.type = tile.typeToBecome;
   }
}


// #TODO - remember last seen feature state
//-----------------------------------------------------------------------------------------------
void Map::Render() const
{
   const Vector2f screenCenter(800.f, 450.f);
   const Vector2f mapSize(m_dimensions.x * 20.f, m_dimensions.y * 20.f);
   const Vector2f startLocation(screenCenter - (mapSize / 2.f));

   const BitmapFont* font = BitmapFont::CreateOrGetFont("CopperplateGothicBold");


   int currentIndex = 0;
   for (const Tile& tile : m_tiles)
   {
      if (tile.isHidden
         || (!m_showAllTiles && !tile.isKnown))
      {
         ++currentIndex;
         continue;
      }

      TileCoords tCoords = GetTileCoordsForIndex(currentIndex);
      float xLocation = startLocation.x + (tCoords.x * 20.f);
      float yLocation = startLocation.y + (tCoords.y * 20.f);

      Vector2f tileLocation(xLocation, yLocation);

      if (tile.IsOccupiedByAgent() 
         && (tile.isVisible || m_showAllTiles))
      {
         Agent* agent = tile.occupyingAgent;
         std::string glyphAsString;
         glyphAsString.push_back(agent->GetGlyph());
         g_theRenderer->DrawText2D(tileLocation, glyphAsString, agent->GetColor(), 30.f, font);
         ++currentIndex;
         continue;
      }

      if (tile.HasItems()
         && (tile.isVisible || m_showAllTiles))
      {
         std::string glyphAsString;
         glyphAsString.push_back(tile.GetItemGlyph());
         g_theRenderer->DrawText2D(tileLocation, glyphAsString, Rgba::YELLOW, 30.f, font);
         ++currentIndex;
         continue;
      }

      if (tile.HasAFeature()
         && (tile.isVisible || m_showAllTiles))
      {
         std::string glyphAsString;
         glyphAsString.push_back(tile.occupyingFeature->GetGlyphForCurrentState());
         g_theRenderer->DrawText2D(tileLocation, glyphAsString, Rgba::YELLOW, 30.f, font);
         ++currentIndex;
         continue;
      }

      unsigned char tileAlpha = tile.isVisible ? 255 : 50;

      if (tile.type == STONE_TYPE)
      {
         g_theRenderer->DrawText2D(tileLocation, "#", Rgba(255, 255, 255, tileAlpha), 30.f, font);
      }
      else if (tile.type == AIR_TYPE)
      {
         g_theRenderer->DrawText2D(tileLocation + Vector2f(5.f, 8.f), ".", Rgba(255, 255, 255, tileAlpha), 30.f, font);
      }
      else if (tile.type == WATER_TYPE)
      {
         g_theRenderer->DrawText2D(tileLocation, "~", Rgba(0, 0, 255, tileAlpha), 30.f, font);
      }

      ++currentIndex;
   }
}


//-----------------------------------------------------------------------------------------------
void Map::RenderPath(const Path& path) const
{
   const Vector2f screenCenter(800.f, 450.f);
   const Vector2f mapSize(m_dimensions.x * 20.f, m_dimensions.y * 20.f);
   const Vector2f startLocation(screenCenter - (mapSize / 2.f));

   // Render open list
   for (const PathNode* const node : path.openList)
   {
      const TileCoords& tCoords = node->position;
      float xLocation = startLocation.x + (tCoords.x * 20.f);
      float yLocation = startLocation.y + (tCoords.y * 20.f);

      Vector2f tileLocation(xLocation, yLocation - 20.f);

      g_theRenderer->DrawAABB2(AABB2(tileLocation, Vector2f(tileLocation.x + 20.f, tileLocation.y + 20.f)), Rgba(0, 0, 100, 120));
   }

   // Render closed list
   for (const PathNode* const node : path.closedList)
   {
      const TileCoords& tCoords = node->position;
      float xLocation = startLocation.x + (tCoords.x * 20.f);
      float yLocation = startLocation.y + (tCoords.y * 20.f);

      Vector2f tileLocation(xLocation, yLocation - 20.f);

      g_theRenderer->DrawAABB2(AABB2(tileLocation, Vector2f(tileLocation.x + 20.f, tileLocation.y + 20.f)), Rgba(100, 0, 0, 120));
   }

   // Render goal
   TileCoords tCoords = path.pathEnd;
   float xLocation = startLocation.x + (tCoords.x * 20.f);
   float yLocation = startLocation.y + (tCoords.y * 20.f);

   Vector2f tileLocation(xLocation, yLocation - 20.f);

   g_theRenderer->DrawAABB2(AABB2(tileLocation, Vector2f(tileLocation.x + 20.f, tileLocation.y + 20.f)), Rgba(0, 100, 0, 120));
}


//-----------------------------------------------------------------------------------------------
void Map::RenderRaycastResult(const Vector2f& start, const Vector2f& end, const RaycastResult& result) const
{
   const Vector2f screenCenter(800.f, 450.f);
   const Vector2f mapSize(m_dimensions.x * 20.f, m_dimensions.y * 20.f);
   const Vector2f startLocation(screenCenter - (mapSize / 2.f));

   float xLocation = startLocation.x + (start.x * 20.f);
   float yLocation = startLocation.y + (start.y * 20.f);
   Vector2f lineStart(xLocation, yLocation);

   xLocation = startLocation.x + (result.impactPos.x * 20.f);
   yLocation = startLocation.y + (result.impactPos.y * 20.f);
   Vector2f lineEnd(xLocation, yLocation);

   Rgba lineColor = result.didImpact ? Rgba::RED : Rgba::GREEN;

   g_theRenderer->DrawLine(lineStart, lineEnd, lineColor, 3.f);

   xLocation = startLocation.x + (end.x * 20.f);
   yLocation = startLocation.y + (end.y * 20.f);
   Vector2f endLoc(xLocation, yLocation);
   g_theRenderer->DrawPoint(endLoc, Rgba::GREEN, 3.f);
}


//-----------------------------------------------------------------------------------------------
Tile* Map::GetTileAtIndex(TileIndex index)
{
   return &(m_tiles[index]);
}


//-----------------------------------------------------------------------------------------------
const Tile* Map::GetTileAtIndex(TileIndex index) const
{
   return &(m_tiles[index]);
}


//-----------------------------------------------------------------------------------------------
Tile* Map::GetTileAtTileCoords(const TileCoords& coords)
{
   TileIndex index = GetIndexForTileCoords(coords);
   Tile* tile = GetTileAtIndex(index);
   return tile;
}


//-----------------------------------------------------------------------------------------------
const Tile* Map::GetTileAtTileCoords(const TileCoords& coords) const
{
   TileIndex index = GetIndexForTileCoords(coords);
   const Tile* tile = GetTileAtIndex(index);
   return tile;
}


//-----------------------------------------------------------------------------------------------
TileIndex Map::GetIndexForTileCoords(const TileCoords& location) const
{
   TileIndex index = (m_dimensions.x * location.y) + location.x;
   return index;
}


//-----------------------------------------------------------------------------------------------
TileCoords Map::GetTileCoordsForIndex(TileIndex index) const
{
   int tileX = index % m_dimensions.x;
   int tileY = index / m_dimensions.x;

   return TileCoords(tileX, tileY);
}


// #TODO - since this doesn't worry about going off map... static?
//-----------------------------------------------------------------------------------------------
TileCoords Map::GetTileCoordsInDirection(const TileCoords& location, TileDirection dir, int distance) const
{
   switch (dir)
   {
   case NORTH:
   {
      TileCoords newLocation(location.x, location.y + distance);
      return newLocation;
   }

   case SOUTH:
   {
      TileCoords newLocation(location.x, location.y - distance);
      return newLocation;
   }

   case EAST:
   {
      TileCoords newLocation(location.x + distance, location.y);
      return newLocation;
   }

   case WEST:
   {
      TileCoords newLocation(location.x - distance, location.y);
      return newLocation;
   }

   case NORTH_WEST:
   {
      TileCoords newLocation(location.x - distance, location.y + distance);
      return newLocation;
   }

   case NORTH_EAST:
   {
      TileCoords newLocation(location.x + distance, location.y + distance);
      return newLocation;
   }

   case SOUTH_WEST:
   {
      TileCoords newLocation(location.x - distance, location.y - distance);
      return newLocation;
   }

   case SOUTH_EAST:
   {
         TileCoords newLocation(location.x + distance, location.y - distance);
         return newLocation;
   }
   }

   // something went wrong
   return location;
}


//-----------------------------------------------------------------------------------------------
const Tile& Map::GetTileInDirection(const TileCoords& location, TileDirection dir, int distance) const
{
   // #TODO - All my offmap checks are wrong?
   // #DEBUG - return invalid for anything off the map
   switch (dir)
   {
   case NORTH:
   {
      if (location.y >= m_dimensions.y - distance)
      {
         return Tile::INVALID_TILE;
      }
      else
      {
         TileCoords newLocation(location.x, location.y + distance);
         TileIndex tileIndex = GetIndexForTileCoords(newLocation);
         return m_tiles[tileIndex];
      }
   }

   case SOUTH:
   {
      if (location.y < distance) // #TODO - location.y - distance > 0 ?
      {
         return Tile::INVALID_TILE;
      }
      else
      {
         TileCoords newLocation(location.x, location.y - distance);
         TileIndex tileIndex = GetIndexForTileCoords(newLocation);
         return m_tiles[tileIndex];
      }
   }

   case EAST:
   {
      if (location.x >= m_dimensions.x - distance)
      {
         return Tile::INVALID_TILE;
      }
      else
      {
         TileCoords newLocation(location.x + distance, location.y);
         TileIndex tileIndex = GetIndexForTileCoords(newLocation);
         return m_tiles[tileIndex];
      }
   }

   case WEST:
   {
      if (location.x < distance)
      {
         return Tile::INVALID_TILE;
      }
      else
      {
         TileCoords newLocation(location.x - distance, location.y);
         TileIndex tileIndex = GetIndexForTileCoords(newLocation);
         return m_tiles[tileIndex];
      }
   }

   case NORTH_WEST:
   {
      if (location.x < distance || location.y > m_dimensions.y - distance)
      {
         return Tile::INVALID_TILE;
      }
      else
      {
         TileCoords newLocation(location.x - distance, location.y + distance);
         TileIndex tileIndex = GetIndexForTileCoords(newLocation);
         return m_tiles[tileIndex];
      }
   }

   case NORTH_EAST:
   {
      if (location.x >= m_dimensions.x - distance || location.y > m_dimensions.y - distance)
      {
         return Tile::INVALID_TILE;
      }
      else
      {
         TileCoords newLocation(location.x + distance, location.y + distance);
         TileIndex tileIndex = GetIndexForTileCoords(newLocation);
         return m_tiles[tileIndex];
      }
   }

   case SOUTH_WEST:
   {
      if (location.x < distance || location.y < distance)
      {
         return Tile::INVALID_TILE;
      }
      else
      {
         TileCoords newLocation(location.x - distance, location.y - distance);
         TileIndex tileIndex = GetIndexForTileCoords(newLocation);
         return m_tiles[tileIndex];
      }
   }

   case SOUTH_EAST:
   {
      if (location.x >= m_dimensions.x - distance || location.y < distance)
      {
         return Tile::INVALID_TILE;
      }
      else
      {
         TileCoords newLocation(location.x + distance, location.y - distance);
         TileIndex tileIndex = GetIndexForTileCoords(newLocation);
         return m_tiles[tileIndex];
      }
   }
   }

   // Something went wrong
   return Tile::INVALID_TILE;
}


// #TODO - off map == stone
//-----------------------------------------------------------------------------------------------
int Map::GetNumberOfTilesOfTypeAroundLocationCircular(const TileCoords& location, TileType type, int radius) const
{
   int numberOfTilesOfTypeAroundLocation = 0;
   TileCoords topLeftTile(location.x - radius, location.y - radius);
   TileCoords botRightTile(location.x + radius, location.y + radius);

   for (int yIndex = topLeftTile.y; yIndex <= botRightTile.y; ++yIndex)
   {
      for (int xIndex = topLeftTile.x; xIndex <= botRightTile.x; ++xIndex)
      {
         TileCoords tileCoordsToCheck(xIndex, yIndex);

         if (AreTileCoordsOffMap(tileCoordsToCheck))
         {
            if (type == STONE_TYPE)
            {
               ++numberOfTilesOfTypeAroundLocation;
            }

            continue;
         }
         
         // skip the center spot
         if (tileCoordsToCheck == location)
         {
            continue;
         }

         const Tile* tileToCheck = GetTileAtTileCoords(tileCoordsToCheck);
         if (tileToCheck->type == type)
         {
            ++numberOfTilesOfTypeAroundLocation;
         }
      }
   }

   return numberOfTilesOfTypeAroundLocation;
}


//-----------------------------------------------------------------------------------------------
int Map::GetNumberOfTilesOfTypeAroundLocationCross(const TileCoords& location, TileType type, int radius) const
{
   int numberOfTilesOfTypeAroundLocation = 0;

   // Loop through JUST N, S, E, and W
   for (int radiusSize = 1; radiusSize <= radius; ++radiusSize)
   {
      for (int directionIndex = 0; directionIndex < NUM_CARDINAL_DIRECTIONS; ++directionIndex)
      {
         const Tile& neighbor = GetTileInDirection(location, (TileDirection)directionIndex, radius);
         if (type == neighbor.type)
         {
            ++numberOfTilesOfTypeAroundLocation;
         }
      }
   }

   return numberOfTilesOfTypeAroundLocation;
}


//-----------------------------------------------------------------------------------------------
int Map::GetNumberOfTilesOfTypeAroundIndex(int index, TileType type, int radius) const
{
   TileCoords indexAsCoords = GetTileCoordsForIndex(index);
   int tileCount = GetNumberOfTilesOfTypeAroundLocationCircular(indexAsCoords, type, radius);
   return tileCount;
}


//-----------------------------------------------------------------------------------------------
bool Map::IsTileIndexOffMap(TileIndex index) const
{
   bool isOffMap = true;
   if (index >= m_tiles.size())
   {
      return isOffMap;
   }

   return !isOffMap;
}


//-----------------------------------------------------------------------------------------------
bool Map::AreTileCoordsOffMap(const TileCoords& coords) const
{
   bool isOffMap = true;

   if (coords.x < 0
      || coords.x >= m_dimensions.x
      || coords.y < 0
      || coords.y >= m_dimensions.y)
   {
      return isOffMap;
   }

   return !isOffMap;
}


//-----------------------------------------------------------------------------------------------
TileCoords Map::GetRandomEdgeCoords() const
{
   bool isTopOrBot = GetRandomTrueOrFalse();

   if (isTopOrBot)
   {
      int y = GetRandomTrueOrFalse() ? 0 : m_dimensions.y - 1;
      int x = GetRandomIntBetweenInclusive(0, m_dimensions.x - 1);
      return TileCoords(x, y);
   }
   else // isLeftOrRightSide
   {
      int x = GetRandomTrueOrFalse() ? 0 : m_dimensions.x - 1;
      int y = GetRandomIntBetweenInclusive(0, m_dimensions.y - 1);
      return TileCoords(x, y);
   }
}


//-----------------------------------------------------------------------------------------------
void Map::SetTilesInBlockToType(const TileCoords& location, TileType type, int radius)
{
   TileCoords topLeftTile(location.x - radius, location.y - radius);
   if (topLeftTile.x < 0)
   {
      topLeftTile.x = 0;
   }

   if (topLeftTile.y < 0)
   {
      topLeftTile.y = 0;
   }

   TileCoords botRightTile(location.x + radius, location.y + radius);
   if (botRightTile.x >= m_dimensions.x)
   {
      botRightTile.x = m_dimensions.x - 1;
   }

   if (botRightTile.y >= m_dimensions.y)
   {
      botRightTile.y = m_dimensions.y - 1;
   }

   for (int yIndex = topLeftTile.y; yIndex <= botRightTile.y; ++yIndex)
   {
      for (int xIndex = topLeftTile.x; xIndex <= botRightTile.x; ++xIndex)
      {
         TileCoords currentCoords(xIndex, yIndex);
         TileIndex index = GetIndexForTileCoords(currentCoords);

         m_tiles[index].typeToBecome = type;
      }
   }
}


//-----------------------------------------------------------------------------------------------
void Map::SetTileAtCoordsToType(const TileCoords& location, TileType type)
{
   TileIndex index = GetIndexForTileCoords(location);
   m_tiles[index].typeToBecome = type;
}


//-----------------------------------------------------------------------------------------------
TileCoords Map::GetRandomOpenCoords() const
{
   TileCoords coords(GetRandomIntBetweenInclusive(0, m_dimensions.x - 1), GetRandomIntBetweenInclusive(0, m_dimensions.y - 1));
   const Tile* tileAtCoords = GetTileAtTileCoords(coords);
   while (tileAtCoords->type == STONE_TYPE || tileAtCoords->IsOccupiedByAgent() || tileAtCoords->HasAFeature())
   {
      coords = TileCoords(GetRandomIntBetweenInclusive(0, m_dimensions.x - 1), GetRandomIntBetweenInclusive(0, m_dimensions.y - 1));
      tileAtCoords = GetTileAtTileCoords(coords);
   }

   return coords;
}


//-----------------------------------------------------------------------------------------------
void Map::AddFeature(Feature* newFeature, const TileCoords& position)
{
   GetTileAtTileCoords(position)->occupyingFeature = newFeature;
}


//-----------------------------------------------------------------------------------------------
bool Map::WriteToXMLNode(XMLNode& parentNode) const
{
   bool mapSaveSuccessful = true;

   XMLNode mapNode = parentNode.addChild("MapData");
   mapNode.addAttribute("mapSize", m_dimensions.ToString().c_str());
   
   XMLNode tileNode = mapNode.addChild("TileData");
   std::string tilesAsString = GetTilesAsString();
   tileNode.addText(tilesAsString.c_str());
   
   XMLNode visibilityNode = mapNode.addChild("VisibilityData");
   std::string visibilityAsString = GetVisibilityAsString();
   visibilityNode.addText(visibilityAsString.c_str());

   return mapSaveSuccessful;
}


//-----------------------------------------------------------------------------------------------
std::string Map::GetTilesAsString() const
{
   std::string tilesAsString("");

   // #TODO - how did I get this far with the tiles not knowing their own glyph?!
   for (int yIndex = m_dimensions.y - 1; yIndex >= 0; --yIndex)
   {
      for (int xIndex = 0; xIndex < m_dimensions.x; ++xIndex)
      {
         TileIndex tileIndex = GetIndexForTileCoords(TileCoords(xIndex, yIndex));
         const Tile* tile = GetTileAtIndex(tileIndex);
         switch (tile->type)
         {
         case AIR_TYPE:
         {
            tilesAsString += '.';
            break;
         }

         case STONE_TYPE:
         {
            tilesAsString += '#';
            break;
         }

         case WATER_TYPE:
         {
            tilesAsString += '~';
            break;
         }
         }
      }

      tilesAsString += '\n';
   }

   return tilesAsString;
}


//-----------------------------------------------------------------------------------------------
std::string Map::GetVisibilityAsString() const
{
   std::string visibilityAsString("");

   // #TODO - how did I get this far with the tiles not knowing their own glyph?!
   for (int yIndex = m_dimensions.y - 1; yIndex >= 0; --yIndex)
   {
      for (int xIndex = 0; xIndex < m_dimensions.x; ++xIndex)
      {
         TileIndex tileIndex = GetIndexForTileCoords(TileCoords(xIndex, yIndex));
         const Tile* tile = GetTileAtIndex(tileIndex);
         
         if (tile->isKnown)
         {
            visibilityAsString += '#';
         }
         else
         {
            visibilityAsString += '`';
         }
      }

      visibilityAsString += '\n';
   }

   return visibilityAsString;
}


//-----------------------------------------------------------------------------------------------
STATIC TileDirection Map::GetOppositeTileDirection(TileDirection originalDirection)
{
   switch (originalDirection)
   {
   case NORTH:
   {
      return SOUTH;
   }

   case SOUTH:
   {
      return NORTH;
   }

   case EAST:
   {
      return WEST;
   }

   case WEST:
   {
      return EAST;
   }

   case NORTH_EAST:
   {
      return SOUTH_WEST;
   }

   case NORTH_WEST:
   {
      return SOUTH_EAST;
   }

   case SOUTH_EAST:
   {
      return NORTH_WEST;
   }

   case SOUTH_WEST:
   {
      return NORTH_EAST;
   }
   }

   // Gets rid of compile warnings
   return NORTH;
}


//-----------------------------------------------------------------------------------------------
STATIC TileDirection Map::GetTileDirection90DegreesClockwise(TileDirection originalDirection)
{
   switch (originalDirection)
   {
   case NORTH:
   {
      return EAST;
   }
   case SOUTH:
   {
      return WEST;
   }
   case EAST:
   {
      return SOUTH;
   }
   case WEST:
   {
      return NORTH;
   }
   case NORTH_WEST:
   {
      return NORTH_EAST;
   }
   case NORTH_EAST:
   {
      return SOUTH_EAST;
   }
   case SOUTH_WEST:
   {
      return NORTH_WEST;
   }
   case SOUTH_EAST:
   {
      return SOUTH_WEST;
   }
   }

   // Something went wrong
   return INVALID_DIRECTION;
}


//-----------------------------------------------------------------------------------------------
STATIC TileDirection Map::GetDirectionFromSourceToDest(const TileCoords& source, const TileCoords& destination)
{
   if (source.x == destination.x)
   {
      if (source.y > destination.y)
      {
         return SOUTH;
      }

      else if (source.y < destination.y)
      {
         return NORTH;
      }
   }
   else if (source.y == destination.y)
   {
      if (source.x > destination.x)
      {
         return WEST;
      }

      else if (source.x < destination.x)
      {
         return EAST;
      }
   }

   else // souce.x != destination.x && souce.y != destination.y
   {
      if (source.x > destination.x
         && source.y > destination.y)
      {
         return SOUTH_WEST;
      }

      else if (source.x > destination.x
         && source.y < destination.y)
      {
         return NORTH_WEST;
      }

      else if (source.x < destination.x
         && source.y > destination.y)
      {
         return SOUTH_EAST;
      }

      else // WARNING - this could be invalid if source == destination
      {
         return NORTH_EAST;
      }
   }

   // should never reach this
   return INVALID_DIRECTION;
}


//-----------------------------------------------------------------------------------------------
STATIC Vector2f Map::GetTileCenterFromTileCoords(const TileCoords& coords)
{
   Vector2f center((float)coords.x + 0.5f, (float)coords.y + 0.5f);
   return center;
}