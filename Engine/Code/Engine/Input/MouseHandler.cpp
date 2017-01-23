#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "Engine/Input/MouseHandler.hpp"
#include "Engine/Input/InputSystem.hpp"


//-----------------------------------------------------------------------------------------------
MouseHandler::MouseHandler()
   : m_isSnapping(false)
   , m_cursorIsShowing(true)
   , m_mouseSnapPosition(Vector2i(0, 0))
   , m_mouseSnapDeviation(Vector2i(0, 0))
   , m_mouseButtonStates()
{
}


//-----------------------------------------------------------------------------------------------
void MouseHandler::Update()
{
   if (m_isSnapping)
   {
      UpdateMouseDeviation();
   }
}


//-----------------------------------------------------------------------------------------------
void MouseHandler::UpdateMouseDeviation()
{
   m_mouseSnapDeviation = GetMousePosition() - m_mouseSnapPosition;
   SetMousePosition(m_mouseSnapPosition);
}


//-----------------------------------------------------------------------------------------------
Vector2i MouseHandler::GetMousePosition() const
{
   POINT winMousePoint;
   GetCursorPos(&winMousePoint);

   Vector2i engineMousePoint;
   engineMousePoint.x = winMousePoint.x;
   engineMousePoint.y = winMousePoint.y;

   return engineMousePoint;
}


//-----------------------------------------------------------------------------------------------
void MouseHandler::SetMousePosition(const Vector2i& newPosition) const
{
   SetCursorPos(newPosition.x, newPosition.y);
}


//-----------------------------------------------------------------------------------------------
void MouseHandler::ResnapMouse()
{
   m_isSnapping = true;
   SetMousePosition(m_mouseSnapPosition);
}


//-----------------------------------------------------------------------------------------------
void MouseHandler::ResnapMouse(const Vector2i& newSnapPosition)
{
   m_isSnapping = true;
   SetMouseSnapPosition(newSnapPosition);
}


//-----------------------------------------------------------------------------------------------
void MouseHandler::UnsnapMouse()
{
   m_isSnapping = false;
}


//-----------------------------------------------------------------------------------------------
void MouseHandler::ShowMouseCursor()
{
   ShowCursor(true);
   m_cursorIsShowing = true;
}


//-----------------------------------------------------------------------------------------------
void MouseHandler::HideMouseCursor()
{
   ShowCursor(false);
   m_cursorIsShowing = false;
}


//-----------------------------------------------------------------------------------------------
void MouseHandler::SetMouseButtonDown(MouseHandler::MouseButton button, int currentFrame)
{
   m_mouseButtonStates[button].m_isDown = true;
   m_mouseButtonStates[button].m_frameNumberChanged = currentFrame;
}


//-----------------------------------------------------------------------------------------------
void MouseHandler::SetMouseButtonUp(MouseHandler::MouseButton button, int currentFrame)
{
   m_mouseButtonStates[button].m_isDown = false;
   m_mouseButtonStates[button].m_frameNumberChanged = currentFrame;
}


//-----------------------------------------------------------------------------------------------
bool MouseHandler::WasMouseButtonJustClicked(MouseHandler::MouseButton button) const
{
   if (m_mouseButtonStates[button].m_isDown
      && m_mouseButtonStates[button].m_frameNumberChanged == InputSystem::GetCurrentFrameNumber())
   {
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
bool MouseHandler::WasMouseButtonJustReleased(MouseHandler::MouseButton button) const
{
   if (!m_mouseButtonStates[button].m_isDown
      && m_mouseButtonStates[button].m_frameNumberChanged == InputSystem::GetCurrentFrameNumber())
   {
      return true;
   }

   return false;
}

