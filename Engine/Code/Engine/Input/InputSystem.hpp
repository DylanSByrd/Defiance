#pragma once

#include "Engine/Input/MouseHandler.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/Vector2i.hpp"


//-----------------------------------------------------------------------------------------------
class InputSystem;
extern InputSystem* g_theInputSystem;


//-----------------------------------------------------------------------------------------------
class InputSystem
{
public:
   InputSystem();
   ~InputSystem() {}

   void Update();
   static int GetCurrentFrameNumber() { return s_currentFrameNumber; }
   static void SetCurrentFrameNumber(int frameNumber) { s_currentFrameNumber = frameNumber; }

   // Mouse
   //-----------------------------------------------------------------------------------------------
   enum MouseWheelScrollDirection
   {
      NO_MOUSE_SCROLL,
      MOUSE_SCROLL_UP,
      MOUSE_SCROLL_DOWN,
      NUM_MOUSE_WHEEL_SCROLL_DIRS
   };

   static const float MOUSE_SENSITIVITY;

   Vector2i GetMousePosition() const;
   const Vector2i& GetMouseSnapDeviation() const;
   void SetMousePosition(const Vector2i& newPosition);
   void SetMouseSnapPosition(const Vector2i& newSnapPosition);
   void ResnapMouse();
   void SnapMouse(const Vector2i& newSnapPosition);
   void UnsnapMouse();
   void ShowMouseCursor();
   void HideMouseCursor();
   void SetMouseButtonDown(MouseHandler::MouseButton button);
   void SetMouseButtonUp(MouseHandler::MouseButton button);
   bool WasMouseButtonJustClicked(MouseHandler::MouseButton button) const;
   bool WasMouseButtonJustReleased(MouseHandler::MouseButton button) const;
   bool IsMouseButtonDown(MouseHandler::MouseButton button) const;
   void SetMouseWheelDelta(short delta);
   bool DidMouseWheelScrollUp() const;
   bool DidMouseWheelScrollDown() const;
   MouseWheelScrollDirection GetMouseWheelScrollDirection() const;


   // Keyboard
   //-----------------------------------------------------------------------------------------------
   static const int NUM_KEYS;
   static const int KEY_DELAY;

   void InitKeyStates();
   void SetKeyDown(unsigned char key);
   void SetKeyUp(unsigned char key);
   int GetFrameNumberKeyLastChanged(unsigned char key) const;
   bool IsKeyDown(unsigned char key) const;
   bool WasKeyJustPressed(unsigned char key) const;
   bool WasKeyJustReleased(unsigned char key) const;
   unsigned char GetLastKeyPressed() const { return m_lastKeyPressed; }
   

   struct KeyButtonState
   {
      int m_frameNumberKeyLastChanged;
      bool m_isKeyDown;
   };


   // Controller
   //-----------------------------------------------------------------------------------------------
   const XboxController& GetController() const { return m_controller; }
   
   
   // Window
   //-----------------------------------------------------------------------------------------------
   void SetWindowHasFocus(bool hasFocus) { m_hasFocus = hasFocus; }
   bool GetWindowHasFocus() const { return m_hasFocus; }


private:
   MouseHandler m_mouseHandler;
   KeyButtonState m_keyStates[256];
   XboxController m_controller;
   bool m_hasFocus;
   int m_frameMouseScrolled;
   unsigned char m_lastKeyPressed;
   static int s_currentFrameNumber;
};