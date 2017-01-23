#pragma once

#include <vector>
#include "Game/Generators/Generator.hpp"


//-----------------------------------------------------------------------------------------------
struct EnvironmentGenerationProcess;
struct XMLNode;
class Vector2i;
typedef Vector2i TileCoords;


//-----------------------------------------------------------------------------------------------
class DungeonGenerator
   : public Generator 
{
public:
   DungeonGenerator(const std::string& name);

   static Generator* CreateGenerator(const std::string& name) { return new DungeonGenerator(name); };
   static EnvironmentGenerationProcess* CreateEnvironmentGenerationProcess(const XMLNode& processNode);

   static GeneratorRegistration s_dungeonGeneratorRegistration;

   void InitializeMap(Map* outMap) const;
   bool GenerateStep(Map* outMap, int* outCurrentStepNumber, EnvironmentGenerationProcess* process = nullptr) const;
   bool TryToMakeRoom(Map* outMap, const TileCoords& hallwayStartCoords, EnvironmentGenerationProcess* process = nullptr) const;

private:
   std::vector<TileCoords> GetEmptyTilesNextToWalls(const Map* const map) const;

   int m_currentRoomCount;
};