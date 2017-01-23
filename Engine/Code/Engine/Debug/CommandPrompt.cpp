#include "Engine/Debug/Console.hpp"
#include "Engine/Debug/CommandPrompt.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Math/AABB2.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const float CommandPrompt::CURSOR_BLINK_TIME = .5f;


//-----------------------------------------------------------------------------------------------
CommandPrompt::CommandPrompt()
   : m_cursorIndex(0)
   , m_isShowingCursor(true)
   , m_timeSinceCursorBlink(0.f)
   , m_currentString("")
   , m_historyIndex(-1)
   , m_commandHistory()
{
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::Update()
{
   UpdateCursor();
   HandleInput();
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::UpdateCursor()
{
   m_timeSinceCursorBlink += Time::GetDeltaSeconds();

   if (m_timeSinceCursorBlink > CURSOR_BLINK_TIME)
   {
      m_timeSinceCursorBlink = 0.f;
      m_isShowingCursor = !m_isShowingCursor;
   }
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::HandleInput()
{
   unsigned char lastKeyPressed = g_theInputSystem->GetLastKeyPressed();

   if (!g_theInputSystem->WasKeyJustPressed(lastKeyPressed))
   {
      return;
   }

   switch (lastKeyPressed)
   {
   case VK_LEFT:
   {
      ShiftCursorIndex(-1);
      break;
   }

   case VK_RIGHT:
   {
      ShiftCursorIndex(1);
      break;
   }

   case VK_UP:
   {
      bool isBack = true;
      ShiftHistoryIndex(isBack);
      break;
   }

   case VK_DOWN:
   {
      bool isBack = true;
      ShiftHistoryIndex(!isBack);
      break;
   }

   case VK_HOME:
   {
      MoveIndexTo(0);
      break;
   }

   case VK_END:
   {
      MoveIndexTo(m_currentString.length());
      break;
   }

   case VK_BACK:
   {
      bool isLeft = true;
      RemoveAChar(isLeft);
      break;
   }

   case VK_DELETE:
   {
      bool isLeft = true;
      RemoveAChar(!isLeft);
      break;
   }

   case VK_RETURN:
   {
      if (m_currentString.length() == 0)
      {
         g_theConsole->CloseConsole();
         break;
      }

      ExecuteCommand();
      SaveCommandToHistory();
      break;
   }

   case VK_ESCAPE:
   {
      if (m_currentString.length() == 0)
      {
         g_theConsole->CloseConsole();
         break;
      }

      ClearPrompt();
      break;
   }

   case VK_NEXT:
   {
      bool isBack = true;
      g_theConsole->ShiftHistoryIndex(isBack);
      break;
   }

   case VK_PRIOR:
   {
      bool isBack = true;
      g_theConsole->ShiftHistoryIndex(!isBack);
      break;
   }
   }
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::PrintChar(unsigned char newChar)
{
   switch (newChar)
   {
      
   // garbage keys
   case VK_BACK:
   case VK_CAPITAL:
   case VK_TAB:
   case '`':
   case 192:
   case 191:
   case 16:
   case 13:
   {
      break;
   }

   default:
   {
      m_currentString.insert(m_currentString.begin() + m_cursorIndex, newChar);
      ++m_cursorIndex;
   }

   }
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::ExecuteCommand()
{
   g_theConsole->ResetHistoryIndex();
   Console::RunCommand(m_currentString);
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::SaveCommandToHistory()
{
   m_commandHistory.emplace(m_commandHistory.begin(), m_currentString);
   m_historyIndex = -1;
   ClearPrompt();
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::ClearPrompt()
{
   MoveIndexTo(0);
   m_currentString.clear();
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::ShiftCursorIndex(int numSpaces)
{
   m_cursorIndex += numSpaces;
   KeepCursorInBounds();
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::MoveIndexTo(int position)
{
   m_cursorIndex = position;
   KeepCursorInBounds();
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::KeepCursorInBounds()
{
   if (m_cursorIndex > (int)m_currentString.length())
   {
      m_cursorIndex = m_currentString.length();
   }
   else if (m_cursorIndex < 0)
   {
      m_cursorIndex = 0;
   }
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::RemoveAChar(bool isLeft)
{
   if (isLeft)
   {
      if (m_cursorIndex == 0)
      {
         return;
      }

      ShiftCursorIndex(-1);
   }
   else // isRight
   {
      if (m_cursorIndex == (int)m_currentString.length())
      {
         return;
      }
   }

   m_currentString.erase(m_cursorIndex, 1);
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::Render() const
{
   RenderOutline();
   RenderCurrentString();
   RenderCursor();
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::RenderOutline() const
{
   Vector2f botLeft(0.f, 0.f);
   Vector2f topRight(1600.f, g_theConsole->GetFontHeight() + 2.f);
   Rgba color = g_theConsole->GetColor();
   g_theRenderer->DrawAABB2(AABB2(botLeft, topRight), color);
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::RenderCursor() const
{
   if (!m_isShowingCursor)
   {
      return;
   }

   std::string subString = m_currentString.substr(0, m_cursorIndex);
   float pixelPos = g_theRenderer->CalcTextWidth(subString, g_theConsole->GetFontHeight(), g_theConsole->GetFont());
   Vector2f bot(pixelPos + 3.f, 1.f);
   Vector2f top(pixelPos + 3.f, g_theConsole->GetFontHeight() + 1.f);
   g_theRenderer->DrawLine(bot, top, Rgba::BLACK);
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::RenderCurrentString() const
{
   float fontHeight = g_theConsole->GetFontHeight();
   const BitmapFont* font = g_theConsole->GetFont();
   g_theRenderer->DrawText2D(Vector2f(3.f, fontHeight + 1.f), m_currentString, Rgba::BLACK, fontHeight, font);
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::ShiftHistoryIndex(bool isBackwards)
{
   if (isBackwards)
   {
      ++m_historyIndex;
      if (m_historyIndex > (int)m_commandHistory.size() - 1)
      {
         m_historyIndex = (int)m_commandHistory.size() - 1;
      }
   }
   else
   {
      --m_historyIndex;
      if (m_historyIndex < -1)
      {
         m_historyIndex = -1;
      }
   }

   LoadCommandAtCurrentHistoryIndex();
}


//-----------------------------------------------------------------------------------------------
void CommandPrompt::LoadCommandAtCurrentHistoryIndex()
{
   if (m_historyIndex == -1)
   {
      ClearPrompt();
      return;
   }

   m_currentString.assign(m_commandHistory[m_historyIndex]);
   m_cursorIndex = m_currentString.size();
}