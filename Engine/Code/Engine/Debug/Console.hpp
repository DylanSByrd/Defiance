#pragma once

#include <list>
#include <map>
#include "Engine/Debug/ConsoleCommandArgs.hpp"
#include "Engine/Debug/ConsoleString.hpp"
#include "Engine/Debug/CommandPrompt.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
class Console;
extern Console* g_theConsole;


//-----------------------------------------------------------------------------------------------
class BitmapFont;
class Vector2f;
class ConsoleCommand;


//-----------------------------------------------------------------------------------------------
class Console
{
public:
   enum DisplayMode
   {
      NO_DISPLAY,
      PROMPT_DISPLAY,
      HISTORY_DISPLAY,
      NUM_DISPLAY_MODES
   };

   Console();
   ~Console() {}

   void Init(const std::string& appName, const std::string& versionInfo);
   void Update();
   void HandleInput();
   void PipeKey(unsigned char key);
   void Render() const;
   void DrawConsole() const;
   void DrawHistory() const;
   void ConsolePrintf(const std::string& string, const Rgba& color);
   void ToggleDisplayMode();
   void SetDisplayMode(DisplayMode mode) { m_displayMode = mode; }
   void CloseConsole();
   void ShiftHistoryIndex(bool isBack);
   void ResetHistoryIndex();
   const Rgba& GetColor() const { return m_color; }
   DisplayMode GetDisplayMode() const { return m_displayMode; }
   float GetFontHeight() const { return m_fontHeight; }
   const BitmapFont* GetFont() const { return m_font; }

   static void RegisterCommand(const std::string& name, const std::string& description, ConsoleCommandFunction* commandFunc);
   static void RunCommand(const std::string& commandString);
   static void ClearLogHistory(ConsoleCommandArgs&);
   static void ShowCommandHelp(ConsoleCommandArgs& args);
   static void ListAllRegisteredCommands();
   static void PrintCommandAndDescription(const std::string& commandName);

   static const int MAX_HISTORY_SIZE;
   static const int HISTORY_SPACING;

   static std::map<std::string, ConsoleCommandFunction*> s_registeredCommands;
   static std::map<std::string, std::string> s_registeredCommandDescriptions;

private:
   std::list<ConsoleString> m_outputHistory;
   DisplayMode m_displayMode;
   Rgba m_color;
   float m_fontHeight;
   BitmapFont* m_font;
   int m_historyDisplayIndex;
   CommandPrompt m_prompt;
};