#pragma once

#include "Engine/Input/XboxController.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
class TheApp;
extern TheApp* g_theApp;


//-----------------------------------------------------------------------------------------------
class TheApp
{
public:
   TheApp();
   ~TheApp() {}

   void Init();
   void Shutdown();
   void Update();
   void Render();
   const int& GetCurrentFrameNumber() const { return m_currentFrameNumber; }
   void AdvanceFrameNumber();
   void SetInputFrameNumber();
   float GetAspectRatio() const;
   
   static void QuitApplication(ConsoleCommandArgs&);
   static void QuitApplication();
   static bool IsQuitting() { return s_isQuitting; }

   static const int WINDOW_PHYSICAL_WIDTH;
   static const int WINDOW_PHYSICAL_HEIGHT;
   static const float VIEW_LEFT;
   static const float VIEW_RIGHT;
   static const float VIEW_BOTTOM;
   static const float VIEW_TOP;
   static const int OFFSET_FROM_WINDOWS_DESKTOP;
   static const char* APP_NAME;
   static const char* VERSION_INFO;
   
   static bool s_isQuitting;

private:
   int m_currentFrameNumber;
};
