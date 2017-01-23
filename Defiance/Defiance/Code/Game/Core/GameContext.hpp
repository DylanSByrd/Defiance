#pragma once

#include <vector>
#include <set>
#include "Game/Entities/Entity.hpp"
//#include <stack>


//-----------------------------------------------------------------------------------------------
class Generator;
class Map;
class EnvironmentBlueprint;
class PathfinderAStar;
class Player;


//-----------------------------------------------------------------------------------------------
struct GameContext
{
   GameContext();
   ~GameContext();

   void UpdateEntities();
   void CleanUpEntities();

   EnvironmentBlueprint* activeEnvironment;
   Generator* activeGenerator;
   Map* activeMap;
   PathfinderAStar* activePathfinder;
   Player* activePlayer;
   TurnOrderMap activeAgents;
   std::vector<Entity*> activeEntities;
   int activeGeneratorStep;
   bool isGenerationAutomatic;
   bool isEnvironmentFinished;
   int numberOfPlayerStepsTaken;
   int numberOfEnemiesSlainByPlayer;
   // std::stack<GameState> gameStateStack;
};