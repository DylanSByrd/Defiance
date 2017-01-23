#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
STATIC extern InputSystem* g_theInputSystem = nullptr;


//Mouse
//-----------------------------------------------------------------------------------------------
STATIC const float InputSystem::MOUSE_SENSITIVITY = 0.22f;


//Keyboard
//-----------------------------------------------------------------------------------------------
STATIC const int InputSystem::NUM_KEYS = 256;
STATIC const int InputSystem::KEY_DELAY = 60;


//App
//-----------------------------------------------------------------------------------------------
STATIC int InputSystem::s_currentFrameNumber = 0;


//-----------------------------------------------------------------------------------------------
InputSystem::InputSystem()
   : m_mouseHandler()
   , m_controller()
   , m_hasFocus(true)
   , m_frameMouseScrolled(0)
   , m_lastKeyPressed(0)
{
   InitKeyStates();
}


//-----------------------------------------------------------------------------------------------
void InputSystem::Update()
{
   m_mouseHandler.Update();
   m_controller.Update();
}


// Mouse
//-----------------------------------------------------------------------------------------------
Vector2i InputSystem::GetMousePosition() const
{
   return m_mouseHandler.GetMousePosition();
}


//-----------------------------------------------------------------------------------------------
const Vector2i& InputSystem::GetMouseSnapDeviation() const
{
   return m_mouseHandler.GetMouseSnapDeviation();
}


//-----------------------------------------------------------------------------------------------
void InputSystem::SetMousePosition(const Vector2i& newPosition)
{
   m_mouseHandler.SetMousePosition(newPosition);
}


//-----------------------------------------------------------------------------------------------
void InputSystem::SetMouseSnapPosition(const Vector2i& newSnapPosition)
{
   m_mouseHandler.SetMouseSnapPosition(newSnapPosition);
}


//-----------------------------------------------------------------------------------------------
void InputSystem::ResnapMouse()
{
   m_mouseHandler.ResnapMouse();
}


//-----------------------------------------------------------------------------------------------
void InputSystem::UnsnapMouse()
{
   m_mouseHandler.UnsnapMouse();
}


//-----------------------------------------------------------------------------------------------
void InputSystem::SnapMouse(const Vector2i& newSnapPosition)
{
   m_mouseHandler.ResnapMouse(newSnapPosition);
}


//-----------------------------------------------------------------------------------------------
void InputSystem::ShowMouseCursor()
{
   m_mouseHandler.ShowMouseCursor();
}


//-----------------------------------------------------------------------------------------------
void InputSystem::HideMouseCursor()
{
   m_mouseHandler.HideMouseCursor();
}


// Keyboard
//-----------------------------------------------------------------------------------------------
void InputSystem::InitKeyStates()
{
   for (int keyIndex = 0; keyIndex < NUM_KEYS; ++keyIndex)
   {
      m_keyStates[keyIndex].m_frameNumberKeyLastChanged = 0;
      m_keyStates[keyIndex].m_isKeyDown = false;
   }
}

//-----------------------------------------------------------------------------------------------
void InputSystem::SetKeyDown(unsigned char key)
{
   m_keyStates[key].m_frameNumberKeyLastChanged = s_currentFrameNumber;
   m_keyStates[key].m_isKeyDown = true;

   m_lastKeyPressed = key;
}


//-----------------------------------------------------------------------------------------------
void InputSystem::SetKeyUp(unsigned char key)
{
   m_keyStates[key].m_frameNumberKeyLastChanged = s_currentFrameNumber;
   m_keyStates[key].m_isKeyDown = false;
}


//-----------------------------------------------------------------------------------------------
int InputSystem::GetFrameNumberKeyLastChanged(unsigned char key) const
{
   return m_keyStates[key].m_frameNumberKeyLastChanged;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::IsKeyDown(unsigned char key) const
{
   return m_keyStates[key].m_isKeyDown;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustPressed(unsigned char key) const
{
   if (!IsKeyDown(key))
   {
      return false;
   }

   return (GetFrameNumberKeyLastChanged(key) == s_currentFrameNumber);
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustReleased(unsigned char key) const
{
   if (IsKeyDown(key))
   {
      return false;
   }

   return (GetFrameNumberKeyLastChanged(key) == s_currentFrameNumber);
}


//-----------------------------------------------------------------------------------------------
void InputSystem::SetMouseButtonDown(MouseHandler::MouseButton button)
{
   m_mouseHandler.SetMouseButtonDown(button, s_currentFrameNumber);
}


//-----------------------------------------------------------------------------------------------
void InputSystem::SetMouseButtonUp(MouseHandler::MouseButton button)
{
   m_mouseHandler.SetMouseButtonUp(button, s_currentFrameNumber);
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::WasMouseButtonJustClicked(MouseHandler::MouseButton button) const
{
   return m_mouseHandler.WasMouseButtonJustClicked(button);
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::WasMouseButtonJustReleased(MouseHandler::MouseButton button) const
{
   return m_mouseHandler.WasMouseButtonJustReleased(button);
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::IsMouseButtonDown(MouseHandler::MouseButton button) const
{
   return m_mouseHandler.IsMouseButtonDown(button);
}


//-----------------------------------------------------------------------------------------------
void InputSystem::SetMouseWheelDelta(short delta)
{
   m_mouseHandler.SetMouseWheelDelta(delta);
   m_frameMouseScrolled = s_currentFrameNumber;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::DidMouseWheelScrollUp() const
{
   if (m_mouseHandler.GetMouseWheelDelta() > 0)
   {
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::DidMouseWheelScrollDown() const
{
   if (m_mouseHandler.GetMouseWheelDelta() < 0)
   {
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
InputSystem::MouseWheelScrollDirection InputSystem::GetMouseWheelScrollDirection() const
{
   if (m_frameMouseScrolled == s_currentFrameNumber)
   {
      if (DidMouseWheelScrollUp())
      {
         return MOUSE_SCROLL_UP;
      }

      if (DidMouseWheelScrollDown())
      {
         return MOUSE_SCROLL_DOWN;
      }
   }

   return NO_MOUSE_SCROLL;
}