#include "Engine/Debug/Console.hpp"
#include "Engine/Debug/ConsoleCommand.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Input/InputSystem.hpp"


//-----------------------------------------------------------------------------------------------
extern Console* g_theConsole = nullptr;


//-----------------------------------------------------------------------------------------------
STATIC const int Console::MAX_HISTORY_SIZE = 15;
STATIC const int Console::HISTORY_SPACING = 4;
STATIC std::map<std::string, ConsoleCommandFunction*> Console::s_registeredCommands;
STATIC std::map<std::string, std::string> Console::s_registeredCommandDescriptions;


//-----------------------------------------------------------------------------------------------
Console::Console()
   : m_outputHistory()
   , m_displayMode(NO_DISPLAY)
   , m_color(Rgba(255, 255, 255, 76))
   , m_fontHeight(24.f)
   , m_font(BitmapFont::CreateOrGetFont("CourierNew"))
   , m_historyDisplayIndex(0)
   , m_prompt()
{
}


//-----------------------------------------------------------------------------------------------
void Console::Init(const std::string& appName, const std::string& versionInfo)
{
   ConsolePrintf(appName, Rgba::BLUE);
   ConsolePrintf(versionInfo, Rgba::BLUE);

   RegisterCommand("clear", "clears the console history", ClearLogHistory);
   RegisterCommand("help", "lists all registered commands. Use \"HELP <commandName>\" for description of the command", ShowCommandHelp);
}


//-----------------------------------------------------------------------------------------------
void Console::Update()
{
   HandleInput();
   if (m_displayMode != NO_DISPLAY)
   {
      m_prompt.Update();
   }
}


//-----------------------------------------------------------------------------------------------
void Console::HandleInput()
{
   if (g_theInputSystem->WasKeyJustPressed(192))
   {
      ToggleDisplayMode();
   }
}


//-----------------------------------------------------------------------------------------------
void Console::PipeKey(unsigned char key)
{
   if (m_displayMode != NO_DISPLAY)
   {
      m_prompt.PrintChar(key);
   }
}


//-----------------------------------------------------------------------------------------------
void Console::Render() const
{
   switch (m_displayMode)
   {
   case NO_DISPLAY:
   {
      return;
   }

   case PROMPT_DISPLAY:
   {
      m_prompt.Render();
      return;
   }

   case HISTORY_DISPLAY:
   {
      m_prompt.Render();
      DrawConsole();
      DrawHistory();
      break;
   }
   }
}


//-----------------------------------------------------------------------------------------------
void Console::DrawConsole() const
{
   float spacePerLine = m_fontHeight + HISTORY_SPACING;
   int numLines = m_outputHistory.size() > MAX_HISTORY_SIZE ? MAX_HISTORY_SIZE : m_outputHistory.size();
   float consoleHeight = numLines * spacePerLine;
   if (consoleHeight == 0)
   {
      consoleHeight = spacePerLine;
   }

   Vector2f botLeft(0.f, spacePerLine);
   Vector2f topRight(1600.f, spacePerLine + consoleHeight);
   Rgba colorToDraw = m_color;
   if (colorToDraw.a > (.3f * 255))
   {
      colorToDraw.a = (char)(.3f * 255);
   }

   g_theRenderer->DrawAABB2(AABB2(botLeft, topRight), colorToDraw);
}


//-----------------------------------------------------------------------------------------------
void Console::DrawHistory() const
{
   Vector2f startPosition(3.f, (2 * m_fontHeight) + HISTORY_SPACING);
   int drawIndex = 0;
   for (const ConsoleString& string : m_outputHistory)
   {
      if (drawIndex > m_historyDisplayIndex + MAX_HISTORY_SIZE - 1)
      {
         break;
      }

      if (drawIndex < m_historyDisplayIndex)
      {
         ++drawIndex;
         continue;
      }

      g_theRenderer->DrawText2D(startPosition, string.content, string.color, m_fontHeight, m_font);

      startPosition.y += (m_fontHeight + HISTORY_SPACING);
      ++drawIndex;
   }
}


// #TODO - make a ConsoleErrorf (red), ConsoleWarningf (yellow?), ConsoleSuccessf (green)
//-----------------------------------------------------------------------------------------------
void Console::ConsolePrintf(const std::string& string, const Rgba& color)
{
   ConsoleString stringToAdd(string, color);
   m_outputHistory.push_front(stringToAdd);
}


//-----------------------------------------------------------------------------------------------
void Console::ToggleDisplayMode()
{
   m_displayMode = (DisplayMode)(m_displayMode + 1);

   if (m_displayMode == NUM_DISPLAY_MODES)
   {
      CloseConsole();
   }
}


//-----------------------------------------------------------------------------------------------
void Console::CloseConsole()
{
   m_displayMode = NO_DISPLAY;
}


//-----------------------------------------------------------------------------------------------
void Console::ShiftHistoryIndex(bool isBack)
{
   if (isBack && m_historyDisplayIndex > 0)
   {
      --m_historyDisplayIndex;
   }
   else if (!isBack && m_historyDisplayIndex < ((int)m_outputHistory.size() - MAX_HISTORY_SIZE))
   {
      ++m_historyDisplayIndex;
   }
}


//-----------------------------------------------------------------------------------------------
void Console::ResetHistoryIndex()
{
   m_historyDisplayIndex = 0;
}


//-----------------------------------------------------------------------------------------------
STATIC void Console::RegisterCommand(const std::string& name, const std::string& description, ConsoleCommandFunction* commandFunc)
{
   s_registeredCommands[name] = commandFunc;
   s_registeredCommandDescriptions[name] = description;
}


//-----------------------------------------------------------------------------------------------
STATIC void Console::RunCommand(const std::string& commandString)
{
   g_theConsole->m_displayMode = HISTORY_DISPLAY;
   ConsoleCommand command;
   command.InitCommandToString(ToLowerCase(commandString));

   auto commandIter = s_registeredCommands.find(command.GetCommandName());
   if (commandIter != s_registeredCommands.end())
   {
      ConsoleCommandFunction* funcToRun = commandIter->second;
      funcToRun(command.GetCommandArgs());
   }
   else
   {
      std::string errorString = Stringf("Error: Command \"%s\" not found.", command.GetCommandName().c_str());
      g_theConsole->ConsolePrintf(errorString, Rgba::RED);
   }
}


//-----------------------------------------------------------------------------------------------
STATIC void Console::ClearLogHistory(ConsoleCommandArgs&)
{
   g_theConsole->m_outputHistory.clear();
}


//-----------------------------------------------------------------------------------------------
STATIC void Console::ShowCommandHelp(ConsoleCommandArgs& args)
{
   if (args.GetArgCount() == 0)
   {
      ListAllRegisteredCommands();
   }

   std::string commandToHelp;
   args.GetNextArgAsString(&commandToHelp, "");

   if (commandToHelp.empty())
   {
      return;
   }

   auto commandIter = s_registeredCommands.find(commandToHelp);
   if (commandIter != s_registeredCommands.end())
   {
      PrintCommandAndDescription(commandToHelp);
   }
   else
   {
      std::string errorString = Stringf("Error - command \"%s\" not found", commandToHelp.c_str());
      g_theConsole->ConsolePrintf(errorString, Rgba::RED);
   }
}


//-----------------------------------------------------------------------------------------------
STATIC void Console::ListAllRegisteredCommands()
{
   g_theConsole->m_displayMode = HISTORY_DISPLAY;
   for (auto commandIter = s_registeredCommands.begin(); commandIter != s_registeredCommands.end(); ++commandIter)
   {
      PrintCommandAndDescription(commandIter->first);
   }
}


//-----------------------------------------------------------------------------------------------
STATIC void Console::PrintCommandAndDescription(const std::string& commandName)
{
   std::string commandNameAndDescription = commandName + " - " + s_registeredCommandDescriptions[commandName];
   g_theConsole->ConsolePrintf(commandNameAndDescription, Rgba::GREEN);
}