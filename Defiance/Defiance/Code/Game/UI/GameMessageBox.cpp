#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/UI/GameMessageBox.hpp"


//-----------------------------------------------------------------------------------------------
extern GameMessageBox* g_theGameMessageBox = nullptr;


//-----------------------------------------------------------------------------------------------
STATIC const int GameMessageBox::MAX_HISTORY_SIZE = 5;
STATIC const int GameMessageBox::HISTORY_SPACING = 4;
STATIC const std::string GameMessageBox::WELCOME_MESSAGE0 = "You wake up, unsure of where you are.";
STATIC const std::string GameMessageBox::WELCOME_MESSAGE1 = "You remember the end of the world, and the life after.";
STATIC const std::string GameMessageBox::WELCOME_MESSAGE2 = "Look for other survivors. Avoid monsters and bandits.";
STATIC const std::string GameMessageBox::WELCOME_MESSAGE3 = "Good Luck.";


//-----------------------------------------------------------------------------------------------
GameMessageBox::GameMessageBox()
   : m_fontHeight(20.f)
   , m_color(Rgba(0, 0, 0, 255))
   , m_font(BitmapFont::CreateOrGetFont("CopperplateGothicBold"))
   , m_historyDisplayIndex(0)
{
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::Init()
{
   PrintNeutralMessage(WELCOME_MESSAGE0);
   PrintNeutralMessage(WELCOME_MESSAGE1);
   PrintNeutralMessage(WELCOME_MESSAGE2);
   PrintNeutralMessage(WELCOME_MESSAGE3);
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::HandleInput()
{
   // PgUP
   if (g_theInputSystem->WasKeyJustPressed(VK_NEXT))
   {
      ShiftHistoryIndexBack();
   }

   // PgDwn
   if (g_theInputSystem->WasKeyJustPressed(VK_PRIOR))
   {
      ShiftHistoryIndexForward();
   }
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::PrintGoodMessage(const std::string& message)
{
   GameMessageString messageWithType = { message, MESSAGE_GOOD };
   m_outputHistory.push_front(messageWithType);
   ResetHistoryIndex();
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::PrintNeutralMessage(const std::string& message)
{
   GameMessageString messageWithType = { message, MESSAGE_NEUTRAL };
   m_outputHistory.push_front(messageWithType);
   ResetHistoryIndex();
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::PrintCautionMessage(const std::string& message)
{
   GameMessageString messageWithType = { message, MESSAGE_CAUTION };
   m_outputHistory.push_front(messageWithType);
   ResetHistoryIndex();
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::PrintDangerMessage(const std::string& message)
{
   GameMessageString messageWithType = { message, MESSAGE_DANGER };
   m_outputHistory.push_front(messageWithType);
   ResetHistoryIndex();
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::ShiftHistoryIndexBack()
{
   if (m_historyDisplayIndex > 0)
   {
      --m_historyDisplayIndex;
   }
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::ShiftHistoryIndexForward()
{
   if (m_historyDisplayIndex < ((int)m_outputHistory.size() - MAX_HISTORY_SIZE))
   {
      ++m_historyDisplayIndex;
   }
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::ResetHistoryIndex()
{
   m_historyDisplayIndex = 0;
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::Render() const
{
   RenderOverlay();
   RenderMessages();
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::RenderOverlay() const
{
   float spacePerLine = m_fontHeight + HISTORY_SPACING;
   float boxHeight = MAX_HISTORY_SIZE * spacePerLine;
   if (boxHeight == 0)
   {
      boxHeight = spacePerLine;
   }

   Vector2f botLeft(400.f, 770.f);
   Vector2f topRight(1550.f, 770.f + boxHeight);
   Rgba colorToDraw = m_color;
   if (colorToDraw.a > (.3f * 255))
   {
      colorToDraw.a = (char)(.3f * 255);
   }

   g_theRenderer->DrawAABB2(AABB2(botLeft, topRight), colorToDraw);
}


//-----------------------------------------------------------------------------------------------
void GameMessageBox::RenderMessages() const
{
   Vector2f startPosition(403.f, 770.f + HISTORY_SPACING + m_fontHeight);
   int drawIndex = 0;
   for (const GameMessageString& string : m_outputHistory)
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

      Rgba colorToRender = GetColorForMessageType(string.type);
      g_theRenderer->DrawText2D(startPosition, string.content, colorToRender, m_fontHeight, m_font);

      startPosition.y += (m_fontHeight + HISTORY_SPACING);
      ++drawIndex;
   }
}


//-----------------------------------------------------------------------------------------------
STATIC Rgba GameMessageBox::GetColorForMessageType(MessageType type)
{
   switch (type)
   {
   case MESSAGE_GOOD:
   {
      return Rgba::GREEN;
   }

   case MESSAGE_NEUTRAL:
   {
      return Rgba::WHITE;
   }

   case MESSAGE_CAUTION:
   {
      return Rgba::YELLOW;
   }

   case MESSAGE_DANGER:
   {
      return Rgba::RED;
   }
   }

   return Rgba::WHITE;
}