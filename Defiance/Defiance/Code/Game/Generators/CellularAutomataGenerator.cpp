#include <vector>
#include "ThirdParty/Parsers/XmlParser.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Generators/CellularAutomataGenerator.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Map/Tile.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"


//-----------------------------------------------------------------------------------------------
STATIC GeneratorRegistration CellularAutomataGenerator::s_cellularAutomataGeneratorRegistration(
   "CellularAutomata", &CellularAutomataGenerator::CreateGenerator, &CellularAutomataGenerator::CreateEnvironmentGenerationProcess);


//-----------------------------------------------------------------------------------------------
CellularAutomataGenerator::CellularAutomataGenerator(const std::string& name)
   : Generator(name)
{
}


//-----------------------------------------------------------------------------------------------
STATIC EnvironmentGenerationProcess* CellularAutomataGenerator::CreateEnvironmentGenerationProcess(const XMLNode& processNode)
{
   return new EnvironmentGenerationProcess(processNode);
}


//-----------------------------------------------------------------------------------------------
void CellularAutomataGenerator::InitializeMap(Map* outMap) const
{
   std::vector<Tile>* mapTiles = outMap->GetAllTiles();

   for (Tile& tile : (*mapTiles))
   {
      bool isAir = GetRandomTrueOrFalseWithProbability(.6f);

      if (isAir)
      {
         tile.type = AIR_TYPE;
         tile.typeToBecome = AIR_TYPE;
      }

      tile.isHidden = false;
   }
}


//-----------------------------------------------------------------------------------------------
bool CellularAutomataGenerator::GenerateStep(Map* outMap, int* outCurrentStepNumber, EnvironmentGenerationProcess*) const
{
   std::vector<Tile>* mapTiles = outMap->GetAllTiles();

   // #TODO - make parameters
   if (*outCurrentStepNumber < 4)
   {
      for (TileIndex tileIndex = 0; tileIndex < mapTiles->size(); ++tileIndex)
      {
         Tile* currentTile = &(mapTiles->at(tileIndex));
         int localStoneTiles = outMap->GetNumberOfTilesOfTypeAroundIndex(tileIndex, STONE_TYPE);
         
         if (localStoneTiles >= 5)
         {
            currentTile->typeToBecome = STONE_TYPE;
         }

         localStoneTiles = outMap->GetNumberOfTilesOfTypeAroundIndex(tileIndex, STONE_TYPE, 2);
         if (localStoneTiles < 2)
         {
            currentTile->typeToBecome = AIR_TYPE;
         }
      }
   }
   else if (*outCurrentStepNumber < 9)
   {
      for (TileIndex tileIndex = 0; tileIndex < mapTiles->size(); ++tileIndex)
      {
         Tile* currentTile = &(mapTiles->at(tileIndex));
         int localStoneTiles = outMap->GetNumberOfTilesOfTypeAroundIndex(tileIndex, STONE_TYPE);

         if (localStoneTiles >= 5)
         {
            currentTile->typeToBecome = STONE_TYPE;
         }
         else
         {
            currentTile->typeToBecome = AIR_TYPE;
         }
      }
   }

   outMap->UpdateAllTilesToNewType();

   *outCurrentStepNumber = *outCurrentStepNumber + 1;

   if (*outCurrentStepNumber > 8)
   {
      return false;
   }

   return true;
}