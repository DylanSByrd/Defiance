#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Audio/TheAudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Debug/DebugRenderer.hpp"
#include "Engine/Debug/Console.hpp"
#include "Game/App/TheApp.hpp"
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
STATIC extern TheApp* g_theApp = nullptr;


//-----------------------------------------------------------------------------------------------
STATIC const int TheApp::WINDOW_PHYSICAL_WIDTH = 1600;
STATIC const int TheApp::WINDOW_PHYSICAL_HEIGHT = 900;
STATIC const float TheApp::VIEW_LEFT = 0.0f;
STATIC const float TheApp::VIEW_RIGHT = 1600.0f;
STATIC const float TheApp::VIEW_BOTTOM = 0.0f;
STATIC const float TheApp::VIEW_TOP = VIEW_RIGHT * float(WINDOW_PHYSICAL_HEIGHT) / float(WINDOW_PHYSICAL_WIDTH);
STATIC const int TheApp::OFFSET_FROM_WINDOWS_DESKTOP = 50;
STATIC const char* TheApp::APP_NAME = "Defiance - Dylan S. Byrd";
STATIC const char* TheApp::VERSION_INFO = "Version: 0.0.1";


//-----------------------------------------------------------------------------------------------
STATIC bool TheApp::s_isQuitting = false;


//-----------------------------------------------------------------------------------------------
TheApp::TheApp()
   : m_currentFrameNumber(0)
{
}


//-----------------------------------------------------------------------------------------------
void TheApp::Init()
{
   g_theConsole = new Console();
   g_theConsole->Init(APP_NAME, VERSION_INFO);
   g_theAudioSystem = new TheAudioSystem();
   g_theInputSystem = new InputSystem();
   SetProcessDPIAware();
   
   g_theRenderer = new TheRenderer();
   bool renderInitSuccess = g_theRenderer->Init();
   if (!renderInitSuccess)
   {
      s_isQuitting = true;
      return;
   }

   g_theDebugRenderer = new DebugRenderer();
   g_theGame = new TheGame();
   g_theGame->Init();

   Console::RegisterCommand("quit", "closes the application", QuitApplication);
}


//-----------------------------------------------------------------------------------------------
void TheApp::Shutdown()
{
   delete g_theAudioSystem;
   delete g_theInputSystem;
   delete g_theDebugRenderer;
   delete g_theConsole;
   delete g_theRenderer;
   delete g_theGame;
}


//-----------------------------------------------------------------------------------------------
void TheApp::Update()
{
   if (!g_theInputSystem->GetWindowHasFocus())
   {
      return;
   }

   Time::Update();
   g_theInputSystem->Update();
   g_theAudioSystem->Update();
   g_theGame->Update();
   g_theConsole->Update();

   SetInputFrameNumber();
   AdvanceFrameNumber();
}


//-----------------------------------------------------------------------------------------------
void TheApp::Render()
{
   g_theGame->Render();
}


//-----------------------------------------------------------------------------------------------
void TheApp::AdvanceFrameNumber()
{
   ++m_currentFrameNumber;
}


//-----------------------------------------------------------------------------------------------
void TheApp::SetInputFrameNumber()
{
   InputSystem::SetCurrentFrameNumber(m_currentFrameNumber);
}


//-----------------------------------------------------------------------------------------------
float TheApp::GetAspectRatio() const
{
   return VIEW_RIGHT / VIEW_TOP;
}


//-----------------------------------------------------------------------------------------------
STATIC void TheApp::QuitApplication(ConsoleCommandArgs&)
{
   s_isQuitting = true;
}


//-----------------------------------------------------------------------------------------------
STATIC void TheApp::QuitApplication()
{
   s_isQuitting = true;
}