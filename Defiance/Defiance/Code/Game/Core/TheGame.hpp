#pragma once

#include <stack>
#include "Game/FieldOfView/FieldOfView.hpp"


//-----------------------------------------------------------------------------------------------
class Texture;
class Framebuffer;
class MeshRenderer;
class ConsoleCommandArgs;
struct GameContext;


//-----------------------------------------------------------------------------------------------
class TheGame;
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
class TheGame
{
public:
   enum GameState
   {
      STARTING_STATE,
      MAIN_MENU_STATE,
      MAP_SELECT_STATE,
      GENERATION_STATE,
      PLAYING_STATE,
      PAUSED_STATE,
      CONFIRM_EXIT_STATE,
      QUIT_STATE,
      NUM_GAME_STATES,
   };

   TheGame();
   ~TheGame();

   void Init();
   void RegisterConsoleCommands();
   
   void InitPlayingUI();
   void CleanUpPlayingUI();

   void Update();
   void UpdateGeneration();
   void UpdatePlaying();

   void HandleInput();
   void HandleInputMainMenu();
   void HandleInputMapSelect();
   void HandleInputGeneration();
   void HandleInputPlaying();
   void HandleInputPaused();
   void HandleInputConfirmExit();

   void Render() const;
   void RenderMainMenu() const;
   void RenderMapSelect() const;
   void RenderGeneration() const;
   void RenderPlaying() const;
   void RenderPathfinder() const;
   void RenderPaused() const;
   void RenderConfirmExit() const;

   void AddPlayerAtRandomLocation();
   void AddRandomNPCs();
   void AddRandomItems();
   void InitPathfinder();
   void RunPathfindingTest();
   void CleanUpDeadEntities();
   void RemoveReferencesToEntity(Entity* entity);
   void ResetGame();
   void IncrementPlayerTurnCount();
   void IncrementPlayerKillCount();
   void SetStateGameOver();

   int GetPlayerStepCount() const;
   bool TryGenerationStep() const;
   bool GameHasFocus() const;
   void ClearScreen() const;
   void SetUpWorldCoordinateSystem() const;
   void Setup2DView() const;
   void DrawDebugInfo() const;

   void CheckForSaveGame();
   void SaveGameToFile();
   void LoadGameFromFile();

   GameContext* GetGameContext() const { return m_gameContext; }

   static void ToggleDebug(ConsoleCommandArgs&);
   static bool s_isDebug;

   static void ToggleGodMode(ConsoleCommandArgs&);
   bool IsGodModeEnabled() const { return m_isGodMode; }

   RaycastResult m_testCast;
   Vector2f m_testTarget;
   bool m_showTestCast;

private:
   float m_simulationClock;
   float m_simulationDelta;
   float m_secondsSinceLastFrame;
   bool m_isGameOver;
   bool m_isGodMode;
   bool m_doesSaveExist;
   bool m_didJustSave;
   GameContext* m_gameContext;

   std::stack<GameState> m_gameStateStack; // #TODO - make this custom like the matrix stack
};
