#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/IO/FileUtils.hpp"
#include "Engine/Debug/DebugRenderer.hpp"
#include "Engine/Debug/Console.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/App/TheApp.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Core/GameContext.hpp"
#include "Game/IO/SaveGame.hpp"
#include "Game/IO/LoadGame.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Generators/Generator.hpp"
#include "Game/Environments/EnvironmentBlueprint.hpp"
#include "Game/Entities/Agents/NPCs/NPCFactory.hpp"
#include "Game/Pathfinding/PathfinderAStar.hpp"
#include "Game/Entities/Agents/Player.hpp"
#include "Game/Entities/Agents/NPCs/NPC.hpp"
#include "Game/Entities/Agents/Behaviors/Behavior.hpp"
#include "Game/Entities/Agents/Factions/Faction.hpp"
#include "Game/Entities/Items/ItemFactory.hpp"
#include "Game/Entities/Features/FeatureFactory.hpp"
#include "Game/UI/GameMessageBox.hpp"
#include "Game/UI/PlayerStatusBar.hpp"


//-----------------------------------------------------------------------------------------------
STATIC bool TheGame::s_isDebug = false;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
   : m_secondsSinceLastFrame(0.f)
   , m_gameContext(nullptr)
   , m_testCast()
   , m_showTestCast(false)
   , m_simulationClock(0.f)
   , m_simulationDelta(0.1f)
   , m_isGameOver(false)
   , m_isGodMode(false)
   , m_doesSaveExist(false)
   , m_didJustSave(false)
{
   m_gameStateStack.push(STARTING_STATE);
}


//-----------------------------------------------------------------------------------------------
TheGame::~TheGame()
{
   CleanUpPlayingUI();
   Texture::FreeAllTextures();
   BitmapFont::FreeAllFonts();
   GeneratorRegistration::FreeAllGeneratorRegistrations();
   EnvironmentBlueprint::CleanUpEnvironmentBlueprints();
   BehaviorRegistration::FreeAllBehaviorRegistrations();
   NPCFactory::FreeAllNPCBlueprints();
   Faction::FreeAllFactions();
   ItemFactory::FreeAllItemBlueprints();
   FeatureFactory::FreeAllFeatureBlueprints();
   delete m_gameContext;
}


//-----------------------------------------------------------------------------------------------
void TheGame::Init()
{
   RegisterConsoleCommands();

   BitmapFont::CreateOrGetFont("CopperplateGothicBold");
   BitmapFont::CreateOrGetFont("CourierNew");

   m_gameContext = new GameContext;

   EnvironmentBlueprint::LoadEnvironmentBlueprints();
   Faction::LoadAllFactions();
   NPCFactory::LoadAllNPCBlueprints();
   ItemFactory::LoadAllItemBlueprints();
   FeatureFactory::LoadAllFeatureBlueprints();

   CheckForSaveGame();
   m_gameStateStack.push(MAIN_MENU_STATE);
}


//-----------------------------------------------------------------------------------------------
void TheGame::RegisterConsoleCommands()
{
   g_theConsole->RegisterCommand("debug", "toggles the current debug state", ToggleDebug);
   g_theConsole->RegisterCommand("god", "toggles God mode for the player", ToggleGodMode);
}


//-----------------------------------------------------------------------------------------------
void TheGame::InitPlayingUI()
{
   g_theGameMessageBox = new GameMessageBox;
   g_theGameMessageBox->Init();

   g_thePlayerStatusBar = new PlayerStatusBar(m_gameContext->activePlayer);
}


//-----------------------------------------------------------------------------------------------
void TheGame::CleanUpPlayingUI()
{
   delete g_theGameMessageBox;
   g_theGameMessageBox = nullptr;

   delete g_thePlayerStatusBar;
   g_thePlayerStatusBar = nullptr;
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update()
{
   if (!GameHasFocus())
   {
      return;
   }

  m_secondsSinceLastFrame = Time::GetDeltaSeconds();

   HandleInput();

   if (m_gameStateStack.top() == GENERATION_STATE)
   {
      UpdateGeneration();
   }

   if (m_gameStateStack.top() == PLAYING_STATE)
   {
      UpdatePlaying();
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdateGeneration()
{
   if (!m_gameContext->isGenerationAutomatic)
   {
      return;
   }


   while (TryGenerationStep());

   delete m_gameContext->activeGenerator;
   m_gameContext->activeGenerator = nullptr;
   m_gameContext->activeGeneratorStep = 0;
   m_gameContext->isEnvironmentFinished = false;

   Generator::FinalizeMap(m_gameContext->activeMap);
   AddPlayerAtRandomLocation();
   AddRandomNPCs();
   AddRandomItems();
   InitPlayingUI();

   m_gameStateStack.push(PLAYING_STATE);
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdatePlaying()
{
   bool isSimulating = true;
   bool advanceTime = true;

   while (isSimulating)
   {
      TurnOrderMapIter agentIter = m_gameContext->activeAgents.begin();
      Agent* agent = agentIter->second;

      if (agentIter->first > m_simulationClock)
      {
         break;
      }

      if (!agent->IsReadyToUpdate())
      {
         advanceTime = false;
         break;
      }

      m_gameContext->activeAgents.erase(agentIter);

      float duration = 0.f;
      if (agent->IsAlive())
      {
         duration = agent->Update();
      }
      
      if (agent->IsAlive())
      {
         m_gameContext->activeAgents.insert(TurnOrderMapPair(m_simulationClock + duration, agent));
      }
   }

   if (advanceTime)
   {
      m_simulationClock += m_simulationDelta;
   }
   CleanUpDeadEntities();

   if (m_showTestCast)
   {
      FieldOfView::Raycast(Map::GetTileCenterFromTileCoords(m_gameContext->activePlayer->GetPosition() + Vector2f(.5f, .5f)), m_testTarget, &m_testCast, m_gameContext->activeMap);
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleInput() 
{
   if (g_theConsole->GetDisplayMode() != Console::NO_DISPLAY)
   {
      return;
   }

   GameState currentState = m_gameStateStack.top();

   switch (currentState)
   {
   case MAIN_MENU_STATE:
   {
      HandleInputMainMenu();
      break;
   }
   
   case MAP_SELECT_STATE:
   {
      HandleInputMapSelect();
      break;
   }
   
   case GENERATION_STATE:
   {
      HandleInputGeneration();
      break;
   }

   case PLAYING_STATE:
   {
      HandleInputPlaying();
      break;
   }

   case PAUSED_STATE:
   {
      HandleInputPaused();
      break;
   }

   case CONFIRM_EXIT_STATE:
   {
      HandleInputConfirmExit();
      break;
   }
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleInputMainMenu() 
{
   if (g_theInputSystem->WasKeyJustPressed('N'))
   {
      m_gameStateStack.push(MAP_SELECT_STATE);
   }

   if (g_theInputSystem->WasKeyJustPressed('C'))
   {
      LoadGameFromFile();
      m_gameStateStack.push(PLAYING_STATE);
   }

   if (g_theInputSystem->WasKeyJustPressed('Q'))
   {
      g_theApp->QuitApplication();
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleInputMapSelect()
{
   if (g_theInputSystem->WasKeyJustPressed(VK_TAB))
   {
      m_gameContext->isGenerationAutomatic = !m_gameContext->isGenerationAutomatic;
   }

   if (g_theInputSystem->WasKeyJustPressed('1'))
   {
      EnvironmentBlueprint* activeEnvironment = EnvironmentBlueprint::s_environmentBlueprintMap->begin()->second;

      EnvironmentGenerationProcess* genProc = nullptr;
      genProc = activeEnvironment->GetGenerationProcesses()[0];
      Generator* activeGenerator = GeneratorRegistration::CreateGeneratorByName(genProc->generatorName);
      Map* activeMap = activeGenerator->GenerateEmptyMap(activeEnvironment->GetSize(), activeEnvironment->GetName());
      activeGenerator->InitializeMap(activeMap);

      m_gameContext->activeEnvironment = activeEnvironment;
      m_gameContext->activeGenerator = activeGenerator;
      m_gameContext->activeMap = activeMap;

      m_gameStateStack.push(GENERATION_STATE);
   }

   if (g_theInputSystem->WasKeyJustPressed('2'))
   {
      EnvironmentBlueprintMap::iterator environIter = EnvironmentBlueprint::s_environmentBlueprintMap->begin();
      ++environIter;
      EnvironmentBlueprint* activeEnvironment = environIter->second;

      EnvironmentGenerationProcess* genProc = nullptr;
      genProc = activeEnvironment->GetGenerationProcesses()[0];
      Generator* activeGenerator = GeneratorRegistration::CreateGeneratorByName(genProc->generatorName);
      Map* activeMap = activeGenerator->GenerateEmptyMap(activeEnvironment->GetSize(), activeEnvironment->GetName());
      activeGenerator->InitializeMap(activeMap);

      m_gameContext->activeEnvironment = activeEnvironment;
      m_gameContext->activeGenerator = activeGenerator;
      m_gameContext->activeMap = activeMap;

      m_gameStateStack.push(GENERATION_STATE);
   }

   if (g_theInputSystem->WasKeyJustPressed('3'))
   {
      EnvironmentBlueprintMap::iterator environIter = EnvironmentBlueprint::s_environmentBlueprintMap->begin();
      ++environIter;
      ++environIter;
      EnvironmentBlueprint* activeEnvironment = environIter->second;

      EnvironmentGenerationProcess* genProc = nullptr;
      genProc = activeEnvironment->GetGenerationProcesses()[0];
      Generator* activeGenerator = GeneratorRegistration::CreateGeneratorByName(genProc->generatorName);
      Map* activeMap = activeGenerator->GenerateEmptyMap(activeEnvironment->GetSize(), activeEnvironment->GetName());
      activeGenerator->InitializeMap(activeMap);

      m_gameContext->activeEnvironment = activeEnvironment;
      m_gameContext->activeGenerator = activeGenerator;
      m_gameContext->activeMap = activeMap;

      m_gameStateStack.push(GENERATION_STATE);
   }

   if (g_theInputSystem->WasKeyJustPressed('4'))
   {
      EnvironmentBlueprintMap::iterator environIter = EnvironmentBlueprint::s_environmentBlueprintMap->begin();
      ++environIter;
      ++environIter;
      ++environIter;
      EnvironmentBlueprint* activeEnvironment = environIter->second;

      EnvironmentGenerationProcess* genProc = nullptr;
      genProc = activeEnvironment->GetGenerationProcesses()[0];
      Generator* activeGenerator = GeneratorRegistration::CreateGeneratorByName(genProc->generatorName);
      Map* activeMap = activeGenerator->GenerateEmptyMap(activeEnvironment->GetSize(), activeEnvironment->GetName());
      activeGenerator->InitializeMap(activeMap);

      m_gameContext->activeEnvironment = activeEnvironment;
      m_gameContext->activeGenerator = activeGenerator;
      m_gameContext->activeMap = activeMap;

      m_gameStateStack.push(GENERATION_STATE);
   }

   if (g_theInputSystem->WasKeyJustPressed('5'))
   {
      EnvironmentBlueprintMap::iterator environIter = EnvironmentBlueprint::s_environmentBlueprintMap->begin();
      ++environIter;
      ++environIter;
      ++environIter;
      ++environIter;
      EnvironmentBlueprint* activeEnvironment = environIter->second;

      EnvironmentGenerationProcess* genProc = nullptr;
      genProc = activeEnvironment->GetGenerationProcesses()[0];
      Generator* activeGenerator = GeneratorRegistration::CreateGeneratorByName(genProc->generatorName);
      Map* activeMap = activeGenerator->GenerateEmptyMap(activeEnvironment->GetSize(), activeEnvironment->GetName());
      activeGenerator->InitializeMap(activeMap);

      m_gameContext->activeEnvironment = activeEnvironment;
      m_gameContext->activeGenerator = activeGenerator;
      m_gameContext->activeMap = activeMap;

      m_gameStateStack.push(GENERATION_STATE);
   }

   if (g_theInputSystem->WasKeyJustPressed(VK_ESCAPE))
   {
      m_gameStateStack.pop();
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleInputGeneration()
{
   if (g_theInputSystem->WasKeyJustPressed(VK_SPACE))
   {
      TryGenerationStep();
   }

   if (g_theInputSystem->WasKeyJustPressed(VK_RETURN))
   {
      delete m_gameContext->activeGenerator;
      m_gameContext->activeGenerator = nullptr;
      m_gameContext->activeGeneratorStep = 0;
      m_gameContext->isEnvironmentFinished = false;

      Generator::FinalizeMap(m_gameContext->activeMap);
      AddPlayerAtRandomLocation();
      AddRandomNPCs();
      AddRandomItems();
      InitPlayingUI();

      m_gameStateStack.push(PLAYING_STATE);
   }

   if (g_theInputSystem->WasKeyJustPressed(VK_ESCAPE))
   {
      delete m_gameContext->activeMap;
      m_gameContext->activeMap = nullptr;
      delete m_gameContext->activeGenerator;
      m_gameContext->activeGenerator = nullptr;
      m_gameContext->activeGeneratorStep = 0;
      m_gameContext->isEnvironmentFinished = false;

      m_gameStateStack.pop();
   }

}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleInputPlaying() 
{
   if (m_didJustSave)
   {
      if (g_theInputSystem->WasKeyJustPressed(VK_RETURN))
      {
         ResetGame();
         CheckForSaveGame();
         while (m_gameStateStack.top() != MAIN_MENU_STATE)
         {
            m_gameStateStack.pop();
         }
      }

      return;
   }

   if (g_theInputSystem->WasKeyJustPressed(VK_ESCAPE))
   {
      if (m_isGameOver)
      {
         ResetGame();
         CheckForSaveGame();
         while (m_gameStateStack.top() != MAIN_MENU_STATE)
         {
            m_gameStateStack.pop();
         }
      }
      else
      {
         m_gameStateStack.push(PAUSED_STATE);
      }
   }

   if (g_theInputSystem->WasKeyJustPressed('M'))
   {
      m_gameContext->activeMap->ToggleShowUnknownTiles();
   }

   if (g_theInputSystem->WasKeyJustPressed('P'))
   {
      if (g_theInputSystem->IsKeyDown(VK_SHIFT))
      {
         RunPathfindingTest();
      }
      else if (m_gameContext->activePathfinder == nullptr)
      {
         InitPathfinder();
      }
      else
      {
         PathfinderResult currentPathfinderStatus = m_gameContext->activePathfinder->GetResult();
         if (currentPathfinderStatus == INCOMPLETE_RESULT)
         {
            m_gameContext->activePathfinder->TakeStep();
         }
         else // PATH_FOUND || NO_PATH
         {
            delete m_gameContext->activePathfinder;
            m_gameContext->activePathfinder = nullptr;
            InitPathfinder();
         }
      }
   }

   if (g_theInputSystem->WasKeyJustPressed('R'))
   {
      m_showTestCast = true;
      m_testTarget = Map::GetTileCenterFromTileCoords(m_gameContext->activeMap->GetRandomOpenCoords());
   }

   if (m_gameContext->activePlayer != nullptr)
   {
      m_gameContext->activePlayer->HandleInput();
   }

   g_theGameMessageBox->HandleInput();
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleInputPaused() 
{
   if (g_theInputSystem->WasKeyJustPressed(VK_ESCAPE))
   {
      m_gameStateStack.pop();
   }

   if (g_theInputSystem->WasKeyJustPressed(VK_BACK))
   {
      m_gameStateStack.push(CONFIRM_EXIT_STATE);
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleInputConfirmExit()
{
   if (g_theInputSystem->WasKeyJustPressed('Q'))
   {
      delete m_gameContext->activeMap;
      m_gameContext->activeMap = nullptr;

      delete m_gameContext->activePathfinder;
      m_gameContext->activePathfinder = nullptr;

      m_gameContext->CleanUpEntities();
      ResetGame();
      while (m_gameStateStack.top() != MAIN_MENU_STATE)
      {
         m_gameStateStack.pop();
      }
   }

   if (g_theInputSystem->WasKeyJustPressed(VK_ESCAPE))
   {
      m_gameStateStack.pop();
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
   g_theRenderer->ClearScreen(Rgba(70, 70, 70, 255), 1.0f);

   Setup2DView();

   const GameState& currentState = m_gameStateStack.top();
   switch (currentState)
   {
   case MAIN_MENU_STATE:
   {
      RenderMainMenu();
      break;
   }

   case MAP_SELECT_STATE:
   {
      RenderMapSelect();
      break;
   }

   case GENERATION_STATE:
   {
      RenderGeneration();
      break;
   }

   case PLAYING_STATE:
   {
      RenderPlaying();
      break;
   }

   case PAUSED_STATE:
   {
      RenderPaused();
      break;
   }

   case CONFIRM_EXIT_STATE:
   {
      RenderConfirmExit();
      break;
   }
   }

   if (s_isDebug)
   {
      DrawDebugInfo();
   }

   g_theConsole->Render();
}


//-----------------------------------------------------------------------------------------------
void TheGame::RenderMainMenu() const
{
   const BitmapFont* font = BitmapFont::CreateOrGetFont("CopperplateGothicBold");
   g_theRenderer->DrawText2D(Vector2f(200.f, 700.f), "Main Menu", Rgba::WHITE, 40.f, font);
   g_theRenderer->DrawText2D(Vector2f(200.f, 600.f), "New Game - N", Rgba::WHITE, 20.f, font);
   
   if (m_doesSaveExist)
   {
      g_theRenderer->DrawText2D(Vector2f(200.f, 560.f), "Continue Game - C", Rgba::WHITE, 20.f, font);

   }
   
   g_theRenderer->DrawText2D(Vector2f(200.f, 275.f), "Quit Game - Q", Rgba::WHITE, 20.f, font);
}


//-----------------------------------------------------------------------------------------------
void TheGame::RenderMapSelect() const
{
   const BitmapFont* font = BitmapFont::CreateOrGetFont("CopperplateGothicBold");
   g_theRenderer->DrawText2D(Vector2f(200.f, 700.f), "Map Select", Rgba::WHITE, 40.f, font);

   EnvironmentBlueprintMap::const_iterator environmentIter = EnvironmentBlueprint::s_environmentBlueprintMap->begin();
   int currentEnvironmentIndex = 0;
   for (environmentIter; environmentIter != EnvironmentBlueprint::s_environmentBlueprintMap->end(); ++environmentIter)
   {
      g_theRenderer->DrawText2D(Vector2f(200.f, 600.f - (45.f * currentEnvironmentIndex)), Stringf("%d - %s", currentEnvironmentIndex + 1, environmentIter->first.c_str()), Rgba::WHITE, 20.f, font);
      ++currentEnvironmentIndex;
   }

   g_theRenderer->DrawText2D(Vector2f(200.f, 275.f), "Tab - ", Rgba::WHITE, 20.f, font);

   if (m_gameContext->isGenerationAutomatic)
   {
      g_theRenderer->DrawText2D(Vector2f(250.f, 275.f), "Automatic Mode On", Rgba::WHITE, 20.f, font);
   }
   else
   {
      g_theRenderer->DrawText2D(Vector2f(250.f, 275.f), "Automatic Mode Off", Rgba(120, 120, 120, 255), 20.f, font);
   }

   g_theRenderer->DrawText2D(Vector2f(500.f, 275.f), "Escape - Back to Main Menu", Rgba::WHITE, 20.f, font);
}


//-----------------------------------------------------------------------------------------------
void TheGame::RenderGeneration() const
{
   const BitmapFont* font = BitmapFont::CreateOrGetFont("CopperplateGothicBold");
   g_theRenderer->DrawText2D(Vector2f(200.f, 850.f), "Map Generation", Rgba::WHITE, 40.f, font);
   g_theRenderer->DrawText2D(Vector2f(200.f, 100.f), "Enter - Play Map", Rgba::WHITE, 20.f, font);

   if (m_gameContext->isEnvironmentFinished)
   {
      g_theRenderer->DrawText2D(Vector2f(400.f, 100.f), "Generation Complete", Rgba::WHITE, 20.f, font);
   }
   else
   {
      if (!m_gameContext->isGenerationAutomatic)
      {
         g_theRenderer->DrawText2D(Vector2f(400.f, 100.f), "Space - Do Generation Step", Rgba::WHITE, 20.f, font);
      }
      else
      {
         g_theRenderer->DrawText2D(Vector2f(400.f, 100.f), "Generating...", Rgba::WHITE, 20.f, font);
      }

   }

   g_theRenderer->DrawText2D(Vector2f(700.f, 100.f), "Escape - Back to Map Select", Rgba::WHITE, 20.f, font);

   m_gameContext->activeMap->Render();
}


//-----------------------------------------------------------------------------------------------
void TheGame::RenderPlaying() const
{
   const BitmapFont* font = BitmapFont::CreateOrGetFont("CopperplateGothicBold");
   g_theRenderer->DrawText2D(Vector2f(200.f, 100.f), "PgUp/PgDwn - Scroll Message History", Rgba::WHITE, 20.f, font);

   m_gameContext->activeMap->Render();

   if (m_gameContext->activePathfinder != nullptr)
   {
      RenderPathfinder();
   }

   if (m_showTestCast)
   {
      m_gameContext->activeMap->RenderRaycastResult(Map::GetTileCenterFromTileCoords(m_gameContext->activePlayer->GetPosition()), m_testTarget, m_testCast);
   }

   g_theGameMessageBox->Render();
   g_thePlayerStatusBar->Render();

   if (!m_isGameOver)
   {
      g_theRenderer->DrawText2D(Vector2f(200.f, 70.f), "Escape - Pause", Rgba::WHITE, 20.f, font);
   }
   else // (m_isGameOver)
   {
      g_theRenderer->DrawText2D(Vector2f(200.f, 70.f), "Escape - Return To Main Menu", Rgba::WHITE, 20.f, font);
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::RenderPaused() const
{
   const BitmapFont* font = BitmapFont::CreateOrGetFont("CopperplateGothicBold");
   g_theRenderer->DrawText2D(Vector2f(200.f, 700.f), "Paused", Rgba::WHITE, 40.f, font);
   g_theRenderer->DrawText2D(Vector2f(200.f, 325.f), "Escape - Return to Playing", Rgba::WHITE, 20.f, font);
   g_theRenderer->DrawText2D(Vector2f(200.f, 275.f), "Backspace - Quit Game", Rgba::WHITE, 20.f, font);
}


//-----------------------------------------------------------------------------------------------
void TheGame::RenderConfirmExit() const
{
   const BitmapFont* font = BitmapFont::CreateOrGetFont("CopperplateGothicBold");
   g_theRenderer->DrawText2D(Vector2f(600.f, 700.f), "Do you really want to quit?", Rgba::WHITE, 40.f, font);
   g_theRenderer->DrawText2D(Vector2f(600.f, 350.f), "Q - Quit", Rgba::WHITE, 20.f, font);
   g_theRenderer->DrawText2D(Vector2f(600.f, 300.f), "Escape - Go Back", Rgba::WHITE, 20.f, font);
}


//-----------------------------------------------------------------------------------------------
int TheGame::GetPlayerStepCount() const
{
   return m_gameContext->numberOfPlayerStepsTaken;
}


//-----------------------------------------------------------------------------------------------
bool TheGame::TryGenerationStep() const
{
   bool didGenerateStep = true;

   EnvironmentBlueprint* currentEnvironment = m_gameContext->activeEnvironment;
   Generator* activeGenerator = m_gameContext->activeGenerator;
   Map* activeMap = m_gameContext->activeMap;
   const std::vector<EnvironmentGenerationProcess*>& genProcs = currentEnvironment->GetGenerationProcesses();
   
   int* stepCount = &m_gameContext->activeGeneratorStep;
   int genProcStepCount = genProcs[0]->numSteps;
   unsigned int genProcToUse = 0;

   for (genProcToUse; genProcToUse < genProcs.size() && genProcStepCount < *stepCount; ++genProcToUse)
   {
      genProcStepCount += genProcs[genProcToUse]->numSteps;
   }

   if (genProcStepCount == *stepCount)
   {
      genProcToUse++;

      if (genProcToUse >= genProcs.size())
      {
         m_gameContext->isEnvironmentFinished = true;
         return !didGenerateStep;
      }
      else
      {
         genProcStepCount += genProcs[genProcToUse]->numSteps;
      }
   }

   // Check which generator to use
   if (activeGenerator->GetName() != genProcs[genProcToUse]->generatorName)
   {
      delete activeGenerator;
      activeGenerator = GeneratorRegistration::CreateGeneratorByName(genProcs[genProcToUse]->generatorName);
      m_gameContext->activeGenerator = activeGenerator;
   }

   activeGenerator->GenerateStep(activeMap, stepCount, genProcs[genProcToUse]);
   
   if (*stepCount >= genProcStepCount)
   {
      genProcToUse++;
      if (genProcToUse >= genProcs.size())
      {
         m_gameContext->isEnvironmentFinished = true;
         return !didGenerateStep;
      }
   }

   return didGenerateStep;
}


//-----------------------------------------------------------------------------------------------
void TheGame::AddPlayerAtRandomLocation()
{
   TileCoords spawnCoords = m_gameContext->activeMap->GetRandomOpenCoords();
   Player* player = new Player(20, AGENT_TYPE, spawnCoords, '@', Rgba::YELLOW, Rgba(70, 70, 70, 255), "Player");
   player->AddToMap(m_gameContext->activeMap, spawnCoords);
   m_gameContext->activePlayer = player;
   m_gameContext->activeAgents.insert(TurnOrderMapPair(0.f, player));
   m_gameContext->activeEntities.push_back(player);
   player->UpdateFOV();
}


//-----------------------------------------------------------------------------------------------
void TheGame::AddRandomNPCs()
{
   for (NPCFactoryMap::iterator npcIter = NPCFactory::s_factoryMap->begin(); npcIter != NPCFactory::s_factoryMap->end(); ++npcIter)
   {
      NPC* npc = npcIter->second->CreateNPC();
      TileCoords spawnCoords = m_gameContext->activeMap->GetRandomOpenCoords();
      npc->AddToMap(m_gameContext->activeMap, spawnCoords);
      m_gameContext->activeAgents.insert(TurnOrderMapPair(.1f, npc));
      m_gameContext->activeEntities.push_back(npc);
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::AddRandomItems()
{
   for (int itemIndex = 0; itemIndex < 20; ++itemIndex)
   {
      Item* item = ItemFactory::CreateRandomItem();
      TileCoords spawnCoords = m_gameContext->activeMap->GetRandomOpenCoords();
      item->AddToMap(m_gameContext->activeMap, spawnCoords);
      m_gameContext->activeEntities.push_back(item);
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::InitPathfinder()
{
   TileCoords goal = m_gameContext->activeMap->GetRandomOpenCoords();
   m_gameContext->activePathfinder = new PathfinderAStar(m_gameContext->activePlayer->GetPosition(), goal, m_gameContext->activeMap);
}


//-----------------------------------------------------------------------------------------------
void TheGame::RunPathfindingTest()
{
   double startTime = Time::GetCurrentTimeSeconds();
   for (int testIndex = 0; testIndex < 10000; ++testIndex)
   {
      TileCoords goal = m_gameContext->activeMap->GetRandomOpenCoords();
      PathfinderAStar finder = PathfinderAStar(m_gameContext->activePlayer->GetPosition(), goal, m_gameContext->activeMap);
      finder.FindPath();
   }
   double elapsedTime = Time::GetCurrentTimeSeconds() - startTime;

   g_theConsole->SetDisplayMode(Console::HISTORY_DISPLAY);
   g_theConsole->ConsolePrintf(Stringf("Time to run 10,000 paths: %f seconds", elapsedTime), Rgba::GREEN);
}


//-----------------------------------------------------------------------------------------------
void TheGame::CleanUpDeadEntities()
{
   std::vector<Entity*>& entities = m_gameContext->activeEntities;

   for (std::vector<Entity*>::iterator entityIter = entities.begin(); entityIter != entities.end();)
   {
      Entity* currentEntity = *entityIter;
      if (!currentEntity->IsAlive())
      {
         RemoveReferencesToEntity(currentEntity);
         delete currentEntity;
         currentEntity = nullptr;
         entityIter = entities.erase(entityIter);
      }
      else
      {
         ++entityIter;
      }
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::RemoveReferencesToEntity(Entity* entityToDereference)
{
   if (entityToDereference->IsAgent())
   {
      // Clear Tile
      Tile* tileToClear = m_gameContext->activeMap->GetTileAtTileCoords(entityToDereference->GetPosition());
      tileToClear->ClearAgent();


      // Remove from turn order
      for (TurnOrderMapIter turnOrderIter = m_gameContext->activeAgents.begin(); turnOrderIter != m_gameContext->activeAgents.end(); ++turnOrderIter)
      {
         Entity* currentEntity = turnOrderIter->second;
         if (currentEntity == entityToDereference)
         {
            m_gameContext->activeAgents.erase(turnOrderIter);
            break;
         }
      }
   }

   // Remove from all entities
   for (Entity* currentEntity : m_gameContext->activeEntities)
   {
      currentEntity->DereferenceEntity(entityToDereference);
   }

   // Remove player from context
   if (entityToDereference->IsPlayer())
   {
      m_gameContext->activePlayer = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::ResetGame()
{
   m_isGameOver = false;
   m_didJustSave = false;

   CleanUpPlayingUI();

   delete m_gameContext;
   m_gameContext = new GameContext;
}


//-----------------------------------------------------------------------------------------------
void TheGame::IncrementPlayerTurnCount()
{
   ++m_gameContext->numberOfPlayerStepsTaken;
}


//-----------------------------------------------------------------------------------------------
void TheGame::IncrementPlayerKillCount()
{
   ++m_gameContext->numberOfEnemiesSlainByPlayer;
}


//-----------------------------------------------------------------------------------------------
void TheGame::SetStateGameOver()
{
   m_isGameOver = true;
   g_thePlayerStatusBar->SetGameOver();
}


//-----------------------------------------------------------------------------------------------
void TheGame::RenderPathfinder() const
{
   m_gameContext->activeMap->RenderPath(m_gameContext->activePathfinder->GetPath());
}


//-----------------------------------------------------------------------------------------------
bool TheGame::GameHasFocus() const
{
   bool gameHasFocus = g_theInputSystem->GetWindowHasFocus();
   return gameHasFocus;
}


//-----------------------------------------------------------------------------------------------
void TheGame::ClearScreen() const
{
   g_theRenderer->ClearScreen(Rgba(70, 70, 70, 255), 1.f);
}


// #TODO - move to engine
//-----------------------------------------------------------------------------------------------
void TheGame::SetUpWorldCoordinateSystem() const
{
   // Flip Z
   g_theRenderer->ScaleView(Vector3f(1.f, 1.f, -1.f));
}


//-----------------------------------------------------------------------------------------------
void TheGame::Setup2DView() const
{
   g_theRenderer->SetOrtho(Vector2f::ZERO, 
      Vector2f(TheApp::VIEW_RIGHT, TheApp::VIEW_TOP));
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawDebugInfo() const
{
   std::string debugFPSText = Stringf("FPS: %f", 1.f / m_secondsSinceLastFrame);
   Vector2f debugFPSTextPos(2.f, 20.f);


   const BitmapFont* debugFont = BitmapFont::CreateOrGetFont("CopperplateGothicBold");
   g_theRenderer->DisableDepthTesting();
   g_theRenderer->DrawText2D(debugFPSTextPos, debugFPSText, Rgba::WHITE, 20.f, debugFont);

   if (m_gameContext->activePlayer != nullptr)
   {
      const TileCoords& playerTile = m_gameContext->activePlayer->GetPosition();
      g_theRenderer->DrawText2D(debugFPSTextPos + Vector2f(200.f, 0.f), Stringf("Player Tile: %d, %d", playerTile.x, playerTile.y), Rgba::WHITE, 20.f, debugFont);
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::CheckForSaveGame()
{
   // #TODO - look for the hardcoded file rather than the entire directory?
   // #TODO - ORRR support multiple saves
   std::vector<std::string> saveFiles = EnumerateFilesInDirectory("Data/Saves", "*.Save.xml");
   if (saveFiles.size() > 0)
   {
      m_doesSaveExist = true;
   }
   else
   {
      m_doesSaveExist = false;
   }
}


//-----------------------------------------------------------------------------------------------
void TheGame::SaveGameToFile()
{
   SaveGame save(m_gameContext);
   save.Save();

   g_theGameMessageBox->PrintGoodMessage("Game saved to Data/Saves/SaveGame.Save.xml!");
   g_theGameMessageBox->PrintGoodMessage("Press Enter to return to Main Menu.");
   m_didJustSave = true;
}


//-----------------------------------------------------------------------------------------------
void TheGame::LoadGameFromFile()
{
   LoadGame load(m_gameContext);
   load.Load();
   m_doesSaveExist = false;

   m_gameContext->activePlayer->UpdateFOV();
   InitPlayingUI();
}


//-----------------------------------------------------------------------------------------------
STATIC void TheGame::ToggleDebug(ConsoleCommandArgs&)
{
   s_isDebug = !s_isDebug;

   if (s_isDebug)
   {
      g_theConsole->ConsolePrintf("Debug Enabled", Rgba::GREEN);
   }
   else
   {
      g_theConsole->ConsolePrintf("Debug Disabled", Rgba::RED);
   }
}


//-----------------------------------------------------------------------------------------------
STATIC void TheGame::ToggleGodMode(ConsoleCommandArgs&)
{
   g_theGame->m_isGodMode = !g_theGame->m_isGodMode;

   if (g_theGame->m_isGodMode)
   {
      g_theConsole->ConsolePrintf("God mode enabled.", Rgba::GREEN);
   }
   else
   {
      g_theConsole->ConsolePrintf("God mode disabled.", Rgba::RED);
   }
}