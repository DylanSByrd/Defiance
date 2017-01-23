#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Core/GameContext.hpp"
#include "Game/Generators/Generator.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Map/Tile.hpp"
#include "Game/Entities/Features/Feature.hpp"


//-----------------------------------------------------------------------------------------------
STATIC GeneratorRegistrationMap* GeneratorRegistration::s_generatorRegistrationMap = nullptr;


//-----------------------------------------------------------------------------------------------
Generator::Generator(const std::string& name)
   : m_name(name)
{}


//-----------------------------------------------------------------------------------------------
Map* Generator::GenerateEmptyMap(const Vector2i& dimensions, const std::string& name) const
{
   std::vector<Tile> tiles;
   tiles.resize(dimensions.x * dimensions.y);

   for (Tile& tile : tiles)
   {
      tile.type = STONE_TYPE;
      tile.typeToBecome = STONE_TYPE;
      tile.isHidden = false;
   }

   Map* map = new Map(tiles, dimensions, name);
   return map;
}


//-----------------------------------------------------------------------------------------------
STATIC void Generator::FinalizeMap(Map* outMap)
{
   std::vector<Tile>* tiles = outMap->GetAllTiles();
   Vector2i mapDimensions = outMap->GetDimensions();

   // Set Boundaries to Stone
   for (int xIndex = 0; xIndex < mapDimensions.x - 1; ++xIndex)
   {
      TileIndex tileIndex = outMap->GetIndexForTileCoords(TileCoords(xIndex, 0));
      (*tiles)[tileIndex].type = STONE_TYPE;
      (*tiles)[tileIndex].typeToBecome = STONE_TYPE;
      
      tileIndex = outMap->GetIndexForTileCoords(TileCoords(xIndex, mapDimensions.y - 1));
      (*tiles)[tileIndex].type = STONE_TYPE;
      (*tiles)[tileIndex].typeToBecome = STONE_TYPE;
   }

   for (int yIndex = 0; yIndex < mapDimensions.y - 1; ++yIndex)
   {
      TileIndex tileIndex = outMap->GetIndexForTileCoords(TileCoords(0, yIndex));
      (*tiles)[tileIndex].type = STONE_TYPE;
      (*tiles)[tileIndex].typeToBecome = STONE_TYPE;

      tileIndex = outMap->GetIndexForTileCoords(TileCoords(mapDimensions.x - 1, yIndex));
      (*tiles)[tileIndex].type = STONE_TYPE;
      (*tiles)[tileIndex].typeToBecome = STONE_TYPE;
   }

   // Set hidden tiles
   // and adjust for features
   for (TileIndex index = 0; index < tiles->size(); ++index)
   {
      TileCoords coordsToCheck = outMap->GetTileCoordsForIndex(index);
      int neighboringStoneTiles = outMap->GetNumberOfTilesOfTypeAroundLocationCircular(coordsToCheck, STONE_TYPE, 1);
      
      if (neighboringStoneTiles == 8)
      {
         (*tiles)[index].isHidden = true;
      }

      (*tiles)[index].isVisible = false;
      (*tiles)[index].isKnown = false;
      
      if ((*tiles)[index].HasAFeature() && (outMap->GetNumberOfTilesOfTypeAroundLocationCross(coordsToCheck, AIR_TYPE) > 2))
      {
         delete (*tiles)[index].occupyingFeature;
         (*tiles)[index].occupyingFeature = nullptr;
      }
      else if ((*tiles)[index].HasAFeature())
      {
         g_theGame->GetGameContext()->activeEntities.push_back((*tiles)[index].occupyingFeature);
      }
   }

   outMap->UpdateAllTilesToNewType();
}


//-----------------------------------------------------------------------------------------------
GeneratorRegistration::GeneratorRegistration(const std::string& name, GeneratorCreationFunc* creationFunc, GenerationProcessCreationFunc* processFunc)
   : m_name(name)
   , m_creationFunc(creationFunc)
   , m_processCreationFunc(processFunc)
{
   ASSERT_OR_DIE(creationFunc != nullptr, Stringf("Error: no creation function provided for generator %s!", m_name.c_str()));
   
   if (!s_generatorRegistrationMap)
   {
      s_generatorRegistrationMap = new GeneratorRegistrationMap;
   }

   ASSERT_OR_DIE(s_generatorRegistrationMap->find(m_name) == s_generatorRegistrationMap->end(), Stringf("Error: generator %s already registered!", m_name.c_str()));
   s_generatorRegistrationMap->insert(GeneratorRegistrationPair(m_name, this));
}


//-----------------------------------------------------------------------------------------------
STATIC Generator* GeneratorRegistration::CreateGeneratorByName(const std::string& name)
{
   GeneratorRegistration* generatorRegistration = nullptr;
   ASSERT_OR_DIE(s_generatorRegistrationMap != nullptr, "Error: Generator registration map failed to initialize!");

   GeneratorRegistrationMap::iterator registrationIter = s_generatorRegistrationMap->find(name);
   ASSERT_OR_DIE(registrationIter != s_generatorRegistrationMap->end(), Stringf("Error: Generator %s not registered!", name.c_str()));

   generatorRegistration = registrationIter->second;
   Generator* generator = nullptr;
   generator = (*generatorRegistration->m_creationFunc)(name);
   return generator;
}


//-----------------------------------------------------------------------------------------------
STATIC EnvironmentGenerationProcess* GeneratorRegistration::CreateEnvironmentGenerationProcessByName(const std::string& name, const XMLNode& node)
{
   GeneratorRegistration* generatorRegistration = nullptr;
   ASSERT_OR_DIE(s_generatorRegistrationMap != nullptr, "Error: Generator registration map failed to initialize!");

   GeneratorRegistrationMap::iterator registrationIter = s_generatorRegistrationMap->find(name);
   ASSERT_OR_DIE(registrationIter != s_generatorRegistrationMap->end(), Stringf("Error: Generator %s not registered!", name.c_str()));

   generatorRegistration = registrationIter->second;
   EnvironmentGenerationProcess* generationProcess = nullptr;
   generationProcess = (*generatorRegistration->m_processCreationFunc)(node);
   return generationProcess;
}


//-----------------------------------------------------------------------------------------------
STATIC void GeneratorRegistration::FreeAllGeneratorRegistrations()
{
   delete s_generatorRegistrationMap;
}