#pragma once

#include "Engine/Math/Vector2i.hpp"

class MouseHandler
{
public:
   enum MouseButton
   {
      LEFT_BUTTON,
      RIGHT_BUTTON,
      NUM_MOUSE_BUTTONS
   };

   MouseHandler();
   ~MouseHandler() {}

   void Update();
   void UpdateMouseDeviation();
   Vector2i GetMousePosition() const;
   const Vector2i& GetMouseSnapDeviation() const { return m_mouseSnapDeviation; }
   void SetMousePosition(const Vector2i& newPosition) const;
   void SetMouseSnapPosition(const Vector2i& newSnapPosition) { m_mouseSnapPosition = newSnapPosition; }
   void SetIsSnapping(bool isSnapping) { m_isSnapping = isSnapping; }
   void ResnapMouse();
   void ResnapMouse(const Vector2i& newSnapPosition);
   void UnsnapMouse();
   void ShowMouseCursor();
   void HideMouseCursor();
   void SetMouseButtonDown(MouseButton button, int currentFrame);
   void SetMouseButtonUp(MouseButton button, int currentFrame);
   bool WasMouseButtonJustClicked(MouseButton button) const;
   bool WasMouseButtonJustReleased(MouseButton button) const;
   void SetMouseWheelDelta(short delta) { m_wheelDelta = delta; }
   short GetMouseWheelDelta() const { return m_wheelDelta; }
   bool IsMouseButtonDown(MouseButton button) const { return m_mouseButtonStates[button].m_isDown; }

   struct MouseButtonState
   {
      bool m_isDown = false;
      int m_frameNumberChanged = 0;
   };

private:
   bool m_isSnapping;
   bool m_cursorIsShowing;
   short m_wheelDelta;
   Vector2i m_mouseSnapPosition;
   Vector2i m_mouseSnapDeviation;
   MouseButtonState m_mouseButtonStates[NUM_MOUSE_BUTTONS];
};