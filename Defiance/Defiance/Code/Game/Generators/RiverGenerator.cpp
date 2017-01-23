#include <vector>
#include "ThirdParty/Parsers/XmlParser.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Noise.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Generators/RiverGenerator.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Map/Tile.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"


//-----------------------------------------------------------------------------------------------
STATIC GeneratorRegistration RiverGenerator::s_riverGeneratorRegistration(
   "River", &RiverGenerator::CreateGenerator, &RiverGenerator::CreateEnvironmentGenerationProcess);


//-----------------------------------------------------------------------------------------------
RiverGenerator::RiverGenerator(const std::string& name)
   : Generator(name)
{
}


//-----------------------------------------------------------------------------------------------
STATIC EnvironmentGenerationProcess* RiverGenerator::CreateEnvironmentGenerationProcess(const XMLNode& processNode)
{
   return new EnvironmentGenerationProcess(processNode);
}


//-----------------------------------------------------------------------------------------------
void RiverGenerator::InitializeMap(Map* outMap) const
{
   std::vector<Tile>* mapTiles = outMap->GetAllTiles();
   for (Tile& tile : (*mapTiles))
   {
      tile.type = STONE_TYPE;
      tile.typeToBecome = STONE_TYPE;
      tile.isHidden = false;
   }
}


//-----------------------------------------------------------------------------------------------
bool RiverGenerator::GenerateStep(Map* outMap, int* outCurrentStepNumber, EnvironmentGenerationProcess*) const
{
   const Vector2i& dimensions = outMap->GetDimensions();
   TileCoords startCoords(GetRandomIntBetweenInclusive(0, dimensions.x - 1), GetRandomIntBetweenInclusive(0, dimensions.y - 1));

   Vector2f currentPreciseCoords(startCoords);
   while (!outMap->AreTileCoordsOffMap(currentPreciseCoords))
   {
      outMap->SetTilesInBlockToType(currentPreciseCoords, WATER_TYPE);
      //outMap->SetTileAtCoordsToType(currentPreciseCoords, WATER);

      float perlinRandom = ComputePerlinNoise2D(currentPreciseCoords, 5.f, 50.f, 8, .5f);
      float angle = RangeMap(perlinRandom, -1.f, 1.f, -360.f, 360.f);

      Vector2f currentDirection(CosDegrees(angle), SinDegrees(angle));
      currentDirection.Normalize();
      currentPreciseCoords += currentDirection;
   }


   // Repeat in reverse
   currentPreciseCoords = startCoords;
   while (!outMap->AreTileCoordsOffMap(currentPreciseCoords))
   {
      outMap->SetTilesInBlockToType(currentPreciseCoords, WATER_TYPE);
      //outMap->SetTileAtCoordsToType(currentPreciseCoords, WATER);

      float perlinRandom = ComputePerlinNoise2D(currentPreciseCoords, 5.f, 50.f, 8, .5f);
      float angle = RangeMap(perlinRandom, -1.f, 1.f, -360.f, 360.f);

      Vector2f currentDirection(CosDegrees(angle), SinDegrees(angle));
      currentDirection.Normalize();
      currentPreciseCoords -= currentDirection;
   }

   outMap->UpdateAllTilesToNewType();
   *outCurrentStepNumber = *outCurrentStepNumber + 1;
   return true;
}