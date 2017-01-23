#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include "Engine/Math/Vector2f.hpp"


//-----------------------------------------------------------------------------------------------
struct XboxButtonState
{
   int m_frameNumberButtonLastPressed;
   bool m_isButtonDown;
};


//-----------------------------------------------------------------------------------------------
enum XBOX_BUTTON
{
   DPAD_UP,
   DPAD_DOWN,
   DPAD_LEFT,
   DPAD_RIGHT,
   START_BUTTON,
   BACK_BUTTON,
   LSTICK_BUTTON,
   RSTICK_BUTTON,
   L_SHOULDER,
   R_SHOULDER,
   A_BUTTON,
   B_BUTTON,
   X_BUTTON,
   Y_BUTTON,
   XBOX_BUTTON_COUNT
};


//-----------------------------------------------------------------------------------------------
enum XBOX_TRIGGER
{
   LEFT_TRIGGER,
   RIGHT_TRIGGER,
   XBOX_TRIGGER_COUNT
};


//-----------------------------------------------------------------------------------------------
enum XBOX_THUMBSTICK
{
   LEFT_THUMBSTICK,
   RIGHT_THUMBSTICK,
   XBOX_THUMBSTICK_COUNT
};


//-----------------------------------------------------------------------------------------------
enum XBOX_CONTROLLER_NUMBER
{
   CONTROLLER_ONE,
   CONTROLLER_TWO,
   CONTROLLER_THREE,
   CONTROLLER_FOUR,
   MAX_CONTROLLER_COUNT
};


//-----------------------------------------------------------------------------------------------
class XboxController
{
public:
   XboxController();
   XboxController(const XBOX_CONTROLLER_NUMBER& controllerNumber);
   ~XboxController() {}

   void Update();
   bool IsButtonDown(const XBOX_BUTTON& button) const;
   int GetFrameNumberButtonLastChanged(const XBOX_BUTTON& button) const;
   bool WasButtonJustPressed(const XBOX_BUTTON& button) const;
   float GetTriggerPressure(const XBOX_TRIGGER& trigger) const;
   const Vector2f& GetThumbstickPosition(const XBOX_THUMBSTICK& thumbstick) const;
   float GetThumbstickRadius(const XBOX_THUMBSTICK& thumbstick) const;
   float GetThumbstickOrientationDegrees(const XBOX_THUMBSTICK& thumbstick) const;

   static float THUMBSTICK_OUTER_DEADZONE;
   static float THUMBSTICK_MAX;
   static float LEFT_THUMBSTICK_INNER_DEADZONE_PERCENT;
   static float RIGHT_THUMBSTICK_INNER_DEADZONE_PERCENT;
   static float THUMBSTICK_OUTER_DEADZONE_PERCENT;
   static float MAX_TRIGGER_PRESSURE;
   static float TRIGGER_DEADZONE_PERCENT;

private:
   void InitControllerState();
   void SetButtonStates(const WORD& allButtonStates);
   void SetTriggerPressures(const BYTE& leftTriggerPressure, const BYTE& rightTriggerPressure);
   void SetThumbstickPositions(const Vector2f& leftStickPos, const Vector2f& rightStickPos);

   XBOX_CONTROLLER_NUMBER m_controllerNumber;
   XboxButtonState m_buttonStates[XBOX_BUTTON_COUNT];
   float m_triggerPressures[XBOX_TRIGGER_COUNT];
   Vector2f m_thumbstickPosition[XBOX_THUMBSTICK_COUNT];
   float m_thumbstickRadius[XBOX_THUMBSTICK_COUNT];
   float m_thumbstickOrientationDegrees[XBOX_THUMBSTICK_COUNT];

};