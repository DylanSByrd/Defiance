#pragma once

#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class CommandPrompt
{
public:
   CommandPrompt();
   ~CommandPrompt() {}

   void Update();
   void UpdateCursor();
   void HandleInput();
   void PrintChar(unsigned char newChar);
   void ExecuteCommand();
   void SaveCommandToHistory();
   void ClearPrompt();
   void ShiftCursorIndex(int numSpaces);
   void MoveIndexTo(int position);
   void KeepCursorInBounds();
   void RemoveAChar(bool isLeft);
   void Render() const;
   void RenderOutline() const;
   void RenderCursor() const;
   void RenderCurrentString() const;
   void ShiftHistoryIndex(bool isBackwards);
   void LoadCommandAtCurrentHistoryIndex();

   static const float CURSOR_BLINK_TIME;

private:
   int m_cursorIndex;
   bool m_isShowingCursor;
   float m_timeSinceCursorBlink;
   std::string m_currentString;
   int m_historyIndex;
   std::vector<std::string> m_commandHistory;
};