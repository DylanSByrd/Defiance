#include "Game/Core/GameContext.hpp"
#include "Game/Generators/Generator.hpp"
#include "Game/Map/Map.hpp"


//-----------------------------------------------------------------------------------------------
GameContext::GameContext()
   : activeEnvironment(nullptr)
   , activeGenerator(nullptr)
   , activeMap(nullptr)
   , activePathfinder(nullptr)
   , activePlayer(nullptr)
   , activeGeneratorStep(0)
   , isGenerationAutomatic(false)
   , isEnvironmentFinished(false)
   , numberOfPlayerStepsTaken(0)
   , numberOfEnemiesSlainByPlayer(0)
{}


//-----------------------------------------------------------------------------------------------
GameContext::~GameContext()
{
   if (activeGenerator != nullptr)
   {
      delete activeGenerator;
   }

   CleanUpEntities();
   
   if (activeMap != nullptr)
   {
      delete activeMap;
   }
}


//-----------------------------------------------------------------------------------------------
void GameContext::UpdateEntities()
{
   for (Entity* entity : activeEntities)
   {
      entity->Update();
   }
}


//-----------------------------------------------------------------------------------------------
void GameContext::CleanUpEntities()
{
   for (Entity* entity : activeEntities)
   {
      if (entity == nullptr)
      {
         continue;
      }

      delete entity;
      entity = nullptr;
   }

   activeEntities.erase(activeEntities.begin(), activeEntities.end());

   activePlayer = nullptr;
}