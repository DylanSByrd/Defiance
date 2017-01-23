#pragma once

#include <string>
#include <list>
#include "Engine/Renderer/Rgba.hpp"


//-----------------------------------------------------------------------------------------------
class GameMessageBox;
extern GameMessageBox* g_theGameMessageBox;


//-----------------------------------------------------------------------------------------------
class BitmapFont;


//-----------------------------------------------------------------------------------------------
class GameMessageBox
{
public:
   enum MessageType
   {
      MESSAGE_GOOD,
      MESSAGE_NEUTRAL,
      MESSAGE_CAUTION,
      MESSAGE_DANGER,
      NUM_MESSAGE_TYPES,
   };

   struct GameMessageString
   {
      std::string content;
      GameMessageBox::MessageType type;
   };

   GameMessageBox();
   ~GameMessageBox() {}

   void Init();
   void HandleInput();

   void PrintGoodMessage(const std::string& message);
   void PrintNeutralMessage(const std::string& message);
   void PrintCautionMessage(const std::string& message);
   void PrintDangerMessage(const std::string& message);

   void ShiftHistoryIndexBack();
   void ShiftHistoryIndexForward();
   void ResetHistoryIndex();

   void Render() const;
   void RenderOverlay() const;
   void RenderMessages() const;

   static Rgba GetColorForMessageType(MessageType type);

   static const int MAX_HISTORY_SIZE;
   static const int HISTORY_SPACING;
   static const std::string WELCOME_MESSAGE0;
   static const std::string WELCOME_MESSAGE1;
   static const std::string WELCOME_MESSAGE2;
   static const std::string WELCOME_MESSAGE3;

private:
   std::list<GameMessageString> m_outputHistory;
   float m_fontHeight;
   Rgba m_color;
   BitmapFont* m_font;
   int m_historyDisplayIndex;
};