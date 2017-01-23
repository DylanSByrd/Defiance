#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector2i.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Core/GameCommon.hpp"
#include "Game/Generators/DungeonGenerator.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"
#include "Game/Entities/Features/FeatureFactory.hpp"
#include "Game/Entities/Features/Feature.hpp"


//-----------------------------------------------------------------------------------------------
STATIC GeneratorRegistration DungeonGenerator::s_dungeonGeneratorRegistration(
   "Dungeon", &DungeonGenerator::CreateGenerator, &DungeonGenerator::CreateEnvironmentGenerationProcess);


//-----------------------------------------------------------------------------------------------
DungeonGenerator::DungeonGenerator(const std::string& name)
   : Generator(name)
{
}


//-----------------------------------------------------------------------------------------------
STATIC EnvironmentGenerationProcess* DungeonGenerator::CreateEnvironmentGenerationProcess(const XMLNode& processNode)
{
   return new EnvironmentGenerationProcess(processNode);
}


//-----------------------------------------------------------------------------------------------
void DungeonGenerator::InitializeMap(Map* outMap) const
{
   // Init to stone
   std::vector<Tile>* tiles = outMap->GetAllTiles();
   for (Tile& tile : *tiles)
   {
      tile.type = STONE_TYPE;
      tile.typeToBecome = STONE_TYPE;
      tile.isHidden = false;
   }
   
   // Find center of map
   const Vector2i& dimensions = outMap->GetDimensions();
   TileCoords center(dimensions.x / 2, dimensions.y / 2);

   // Build Initial Room
   // #TODO - create default room limits to replace the 3s
   for (int widthIndex = -3; widthIndex <= 3; ++widthIndex)
   {
      for (int heightIndex = -3; heightIndex <= 3; ++heightIndex)
      {
         TileCoords locationToChange(center.x + widthIndex, center.y + heightIndex);
         TileIndex index = outMap->GetIndexForTileCoords(locationToChange);
         tiles->at(index).typeToBecome = AIR_TYPE;
      }
   }

   outMap->UpdateAllTilesToNewType();
}


//-----------------------------------------------------------------------------------------------
bool DungeonGenerator::GenerateStep(Map* outMap, int* outCurrentStepNumber, EnvironmentGenerationProcess* process) const
{
   // Pick random tileNextToWall
   std::vector<TileCoords> tilesNextToWalls = GetEmptyTilesNextToWalls(outMap);
   while (tilesNextToWalls.size() != 0)
   {
      int indexToTry = GetRandomIndexFromCollection(tilesNextToWalls);
      TileCoords hallwayStartCoords = tilesNextToWalls[indexToTry];
      tilesNextToWalls.erase(tilesNextToWalls.begin() + indexToTry);

      if (TryToMakeRoom(outMap, hallwayStartCoords, process))
      {
         *outCurrentStepNumber = *outCurrentStepNumber + 1;
         outMap->UpdateAllTilesToNewType();
         return true;
      }
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
bool DungeonGenerator::TryToMakeRoom(Map* outMap, const TileCoords& hallwayStartCoords, EnvironmentGenerationProcess*) const
{
   bool isRoomSuccessful = true;

   // N, S, E, or W
   std::vector<TileCoords> wallTiles;
   for (int tileIndex = 0; tileIndex < NUM_CARDINAL_DIRECTIONS; ++tileIndex)
   {
      TileCoords neighborCoords = outMap->GetTileCoordsInDirection(hallwayStartCoords, (TileDirection)tileIndex);
      if (!outMap->AreTileCoordsOffMap(neighborCoords)
         && outMap->GetTileAtTileCoords(neighborCoords)->type == STONE_TYPE)
      {
         wallTiles.push_back(neighborCoords);
      }
   }

   TileCoords wallToDig = GetRandomFromCollection(wallTiles);
   TileDirection hallwayDirection = Map::GetDirectionFromSourceToDest(hallwayStartCoords, wallToDig);
   
   // #TODO - make parameter
   const int minLength = 3;
   const int maxLength = 7;
   int hallwayLength = GetRandomIntBetweenInclusive(minLength, maxLength);

   TileCoords hallwayEndCoords = outMap->GetTileCoordsInDirection(hallwayStartCoords, hallwayDirection, hallwayLength);
   while (outMap->AreTileCoordsOffMap(hallwayEndCoords) && hallwayLength > minLength)
   {
      hallwayLength -= 1;
      hallwayEndCoords = outMap->GetTileCoordsInDirection(hallwayStartCoords, hallwayDirection, hallwayLength);
   }

   // We won't go below min length
   if (hallwayLength == minLength && outMap->AreTileCoordsOffMap(hallwayEndCoords))
   {
      return !isRoomSuccessful;
   }

   // Room Logic
   const int minWidth = 3;
   const int maxWidth = 6;
   const int minDepth = 4;
   const int maxDepth = 7;

   int targetRoomWidth = GetRandomIntBetweenInclusive(minWidth, maxWidth);
   int targetRoomDepth = GetRandomIntBetweenInclusive(minDepth, maxDepth);

   int currentRoomWidth = 1; // 1 because hallway
   int currentRoomDepth = 0;

   // See how far we can go to the right
   TileDirection roomRightDirection = Map::GetTileDirection90DegreesClockwise(hallwayDirection);
   int distanceToRight = GetRandomIntBetweenInclusive(0, targetRoomWidth - currentRoomWidth);
   while (outMap->AreTileCoordsOffMap(outMap->GetTileCoordsInDirection(hallwayEndCoords, roomRightDirection, distanceToRight)))
   {
      distanceToRight -= 1;
   }
   currentRoomWidth += distanceToRight;

   int distanceToLeft = targetRoomWidth - currentRoomWidth;
   TileDirection roomLeftDirection = Map::GetOppositeTileDirection(roomRightDirection);
   while (outMap->AreTileCoordsOffMap(outMap->GetTileCoordsInDirection(hallwayEndCoords, roomLeftDirection, distanceToLeft)))
   {
      distanceToLeft -= 1;
   }
   currentRoomWidth += distanceToLeft;

   // too narrow
   if (currentRoomWidth < minWidth)
   {
      return !isRoomSuccessful;
   }

   // Add depth
   currentRoomDepth = targetRoomDepth;
   while (outMap->AreTileCoordsOffMap(outMap->GetTileCoordsInDirection(hallwayEndCoords, hallwayDirection, currentRoomDepth)))
   {
      currentRoomDepth -= 1;
   }

   // too shallow
   if (currentRoomDepth < minDepth)
   {
      return !isRoomSuccessful;
   }

   // We can build the hallway and room!
   // Hallway
   if (outMap->GetNumberOfTilesOfTypeAroundLocationCross(wallToDig, AIR_TYPE) < 2)
   {
      Feature* newDoor = FeatureFactory::CreateRandomFeatureOfType(DOOR_FEATURE);
      newDoor->AddToMap(outMap, wallToDig);
   }

   std::vector<Tile>* mapTiles = outMap->GetAllTiles();
   TileCoords currentTileCoords = hallwayStartCoords;
   while (currentTileCoords != hallwayEndCoords)
   {
      currentTileCoords = outMap->GetTileCoordsInDirection(currentTileCoords, hallwayDirection);
      TileIndex index = outMap->GetIndexForTileCoords(currentTileCoords);
      (*mapTiles)[index].typeToBecome = AIR_TYPE;
   }

   // Room
   TileCoords currentRoomTile = outMap->GetTileCoordsInDirection(hallwayEndCoords, roomRightDirection, distanceToRight);
   for (int widthIndex = 1; widthIndex <= currentRoomWidth; ++widthIndex)
   {
      for (int depthIndex = 1; depthIndex <= currentRoomDepth; ++depthIndex)
      {
         TileCoords tileToChange = outMap->GetTileCoordsInDirection(currentRoomTile, hallwayDirection, depthIndex);
         TileIndex index = outMap->GetIndexForTileCoords(tileToChange);
         (*mapTiles)[index].typeToBecome = AIR_TYPE;
      }
      currentRoomTile = outMap->GetTileCoordsInDirection(currentRoomTile, roomLeftDirection, 1);
   }

   return isRoomSuccessful;
}


//-----------------------------------------------------------------------------------------------
std::vector<TileCoords> DungeonGenerator::GetEmptyTilesNextToWalls(const Map* const map) const
{
   const std::vector<Tile>& tiles = map->GetAllTiles();
   const unsigned int tilesInMap = tiles.size();

   std::vector<TileCoords> tilesNextToWalls;
   for (TileIndex tileIndex = 0; tileIndex < tilesInMap; ++tileIndex)
   {
      // If not air, we skip
      if (tiles[tileIndex].type != AIR_TYPE)
      {
         continue;
      }

      TileCoords tCoords = map->GetTileCoordsForIndex(tileIndex);
      int neighborStoneTiles = map->GetNumberOfTilesOfTypeAroundLocationCross(tCoords, STONE_TYPE, 1);

      if (neighborStoneTiles != 0)
      {
         tilesNextToWalls.push_back(tCoords);
      }
   }

   return tilesNextToWalls;
}