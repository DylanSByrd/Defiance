#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#pragma comment (lib, "xinput9_1_0") // #Eiserloh: Explicitly use XInput v9_1_0, since v1_4 is not supported under Windows 7


//-----------------------------------------------------------------------------------------------
STATIC float XboxController::THUMBSTICK_OUTER_DEADZONE = 32000.f;
STATIC float XboxController::THUMBSTICK_MAX = 32768.f;
STATIC float XboxController::LEFT_THUMBSTICK_INNER_DEADZONE_PERCENT = float(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) / THUMBSTICK_MAX;
STATIC float XboxController::RIGHT_THUMBSTICK_INNER_DEADZONE_PERCENT = float(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) / THUMBSTICK_MAX;
STATIC float XboxController::THUMBSTICK_OUTER_DEADZONE_PERCENT = THUMBSTICK_OUTER_DEADZONE / THUMBSTICK_MAX;
STATIC float XboxController::MAX_TRIGGER_PRESSURE = 255.f;
STATIC float XboxController::TRIGGER_DEADZONE_PERCENT = float(XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / MAX_TRIGGER_PRESSURE;


//-----------------------------------------------------------------------------------------------
XboxController::XboxController()
{
   m_controllerNumber = CONTROLLER_ONE;
   InitControllerState();
}


//-----------------------------------------------------------------------------------------------
XboxController::XboxController(const XBOX_CONTROLLER_NUMBER& controllerNumber)
{
   m_controllerNumber = controllerNumber;
   InitControllerState();
}


//-----------------------------------------------------------------------------------------------
void XboxController::InitControllerState()
{
   for (int i = 0; i < XBOX_BUTTON_COUNT; ++i)
   {
      m_buttonStates[i].m_isButtonDown = false;
      m_buttonStates[i].m_frameNumberButtonLastPressed = -1;
   }

   for (int i = 0; i < XBOX_TRIGGER_COUNT; ++i)
   {
      m_triggerPressures[i] = 0;
   }

   for (int i = 0; i < XBOX_THUMBSTICK_COUNT; ++i)
   {
      m_thumbstickPosition[i] = Vector2f::ZERO;
   }
}


//-----------------------------------------------------------------------------------------------
void XboxController::Update()
{
   XINPUT_STATE xboxControllerState;
   memset(&xboxControllerState, 0, sizeof(xboxControllerState));
   DWORD errorStatus = XInputGetState(m_controllerNumber, &xboxControllerState);

   if (errorStatus == ERROR_SUCCESS)
   {
      SetButtonStates(xboxControllerState.Gamepad.wButtons);

      BYTE leftTriggerPressure = xboxControllerState.Gamepad.bLeftTrigger;
      BYTE rightTriggerPressure = xboxControllerState.Gamepad.bRightTrigger;
      SetTriggerPressures(leftTriggerPressure, rightTriggerPressure);

      Vector2f leftThumbstickPos(xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY);
      Vector2f rightThumbstickPos(xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);
      SetThumbstickPositions(leftThumbstickPos, rightThumbstickPos);
   }
}


//-----------------------------------------------------------------------------------------------
void XboxController::SetButtonStates(const WORD& allButtonStates)
{
   if ((allButtonStates & 0x1) == 0x1)
   {
      if (m_buttonStates[DPAD_UP].m_isButtonDown == false)
      {
         m_buttonStates[DPAD_UP].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[DPAD_UP].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[DPAD_UP].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x2) == 0x2)
   {
      if (m_buttonStates[DPAD_DOWN].m_isButtonDown == false)
      {
         m_buttonStates[DPAD_DOWN].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[DPAD_DOWN].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[DPAD_DOWN].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x4) == 0x4)
   {
      if (m_buttonStates[DPAD_LEFT].m_isButtonDown == false)
      {
         m_buttonStates[DPAD_LEFT].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[DPAD_LEFT].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[DPAD_LEFT].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x8) == 0x8)
   {
      if (m_buttonStates[DPAD_RIGHT].m_isButtonDown == false)
      {
         m_buttonStates[DPAD_RIGHT].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[DPAD_RIGHT].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[DPAD_RIGHT].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x10) == 0x10)
   {
      if (m_buttonStates[START_BUTTON].m_isButtonDown == false)
      {
         m_buttonStates[START_BUTTON].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[START_BUTTON].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[START_BUTTON].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x20) == 0x20)
   {
      if (m_buttonStates[BACK_BUTTON].m_isButtonDown == false)
      {
         m_buttonStates[BACK_BUTTON].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[BACK_BUTTON].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[BACK_BUTTON].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x40) == 0x40)
   {
      if (m_buttonStates[LSTICK_BUTTON].m_isButtonDown == false)
      {
         m_buttonStates[LSTICK_BUTTON].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[LSTICK_BUTTON].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[LSTICK_BUTTON].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x80) == 0x80)
   {
      if (m_buttonStates[RSTICK_BUTTON].m_isButtonDown == false)
      {
         m_buttonStates[RSTICK_BUTTON].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[RSTICK_BUTTON].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[RSTICK_BUTTON].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x100) == 0x100)
   {
      if (m_buttonStates[L_SHOULDER].m_isButtonDown == false)
      {
         m_buttonStates[L_SHOULDER].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[L_SHOULDER].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[L_SHOULDER].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x200) == 0x200)
   {
      if (m_buttonStates[R_SHOULDER].m_isButtonDown == false)
      {
         m_buttonStates[R_SHOULDER].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[R_SHOULDER].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[R_SHOULDER].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x1000) == 0x1000)
   {
      if (m_buttonStates[A_BUTTON].m_isButtonDown == false)
      {
         m_buttonStates[A_BUTTON].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[A_BUTTON].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[A_BUTTON].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x2000) == 0x2000)
   {
      if (m_buttonStates[B_BUTTON].m_isButtonDown == false)
      {
         m_buttonStates[B_BUTTON].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[B_BUTTON].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[B_BUTTON].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x4000) == 0x4000)
   {
      if (m_buttonStates[X_BUTTON].m_isButtonDown == false)
      {
         m_buttonStates[X_BUTTON].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[X_BUTTON].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[X_BUTTON].m_isButtonDown = false;
   }

   if ((allButtonStates & 0x8000) == 0x8000)
   {
      if (m_buttonStates[Y_BUTTON].m_isButtonDown == false)
      {
         m_buttonStates[Y_BUTTON].m_frameNumberButtonLastPressed = InputSystem::GetCurrentFrameNumber();
         m_buttonStates[Y_BUTTON].m_isButtonDown = true;
      }
   }
   else
   {
      m_buttonStates[Y_BUTTON].m_isButtonDown = false;
   }
}


//-----------------------------------------------------------------------------------------------
void XboxController::SetTriggerPressures(const BYTE& leftTriggerPressure, const BYTE& rightTriggerPressure)
{
   float normLeftPressure = float(leftTriggerPressure) / MAX_TRIGGER_PRESSURE;
   float normRightPressure = float(rightTriggerPressure) / MAX_TRIGGER_PRESSURE;

   float leftMappedPressure = RangeMap(normLeftPressure, TRIGGER_DEADZONE_PERCENT, 1.f, 0.f, 1.f);
   float leftClampedPressure = ClampZeroToOnef(leftMappedPressure);

   float rightMappedPressure = RangeMap(normRightPressure, TRIGGER_DEADZONE_PERCENT, 1.f, 0.f, 1.f);
   float rightClampedPressure = ClampZeroToOnef(rightMappedPressure);

   m_triggerPressures[LEFT_TRIGGER] = leftClampedPressure;
   m_triggerPressures[RIGHT_TRIGGER] = rightClampedPressure;
}


//-----------------------------------------------------------------------------------------------
void XboxController::SetThumbstickPositions(const Vector2f& leftStickPos, const Vector2f& rightStickPos)
{
   float normLXPos = float(leftStickPos.x) / THUMBSTICK_MAX;
   float normLYPos = float(leftStickPos.y) / THUMBSTICK_MAX;
   float normRXPos = float(rightStickPos.x) / THUMBSTICK_MAX;
   float normRYPos = float(rightStickPos.y) / THUMBSTICK_MAX;
   
   float leftRadius = 0;
   float leftThetaRadians = 0;
   float rightRadius = 0;
   float rightThetaRadians = 0;
   ConvertCartesianToPolar(normLXPos, normLYPos, &leftRadius, &leftThetaRadians);
   ConvertCartesianToPolar(normRXPos, normRYPos, &rightRadius, &rightThetaRadians);
   
   m_thumbstickOrientationDegrees[LEFT_THUMBSTICK] = RadiansToDegrees(leftThetaRadians);
   m_thumbstickOrientationDegrees[RIGHT_THUMBSTICK] = RadiansToDegrees(rightThetaRadians);

   float leftMappedRadius = RangeMap(leftRadius, LEFT_THUMBSTICK_INNER_DEADZONE_PERCENT, THUMBSTICK_OUTER_DEADZONE_PERCENT, 0.f, 1.f);
   m_thumbstickRadius[LEFT_THUMBSTICK] = ClampZeroToOnef(leftMappedRadius);

   float rightMappedRadius = RangeMap(rightRadius, RIGHT_THUMBSTICK_INNER_DEADZONE_PERCENT, THUMBSTICK_OUTER_DEADZONE_PERCENT, 0.f, 1.f);
   m_thumbstickRadius[RIGHT_THUMBSTICK] = ClampZeroToOnef(rightMappedRadius);
   
   ConvertPolarToCartesian(m_thumbstickRadius[LEFT_THUMBSTICK], leftThetaRadians, &m_thumbstickPosition[LEFT_THUMBSTICK].x, &m_thumbstickPosition[LEFT_THUMBSTICK].y);
   ConvertPolarToCartesian(m_thumbstickRadius[RIGHT_THUMBSTICK], rightThetaRadians, &m_thumbstickPosition[RIGHT_THUMBSTICK].x, &m_thumbstickPosition[RIGHT_THUMBSTICK].y);
}


//-----------------------------------------------------------------------------------------------
bool XboxController::IsButtonDown(const XBOX_BUTTON& button) const
{
   return m_buttonStates[button].m_isButtonDown;
}


//-----------------------------------------------------------------------------------------------
int XboxController::GetFrameNumberButtonLastChanged(const XBOX_BUTTON& button) const
{
   return m_buttonStates[button].m_frameNumberButtonLastPressed;
}


//-----------------------------------------------------------------------------------------------
bool XboxController::WasButtonJustPressed(const XBOX_BUTTON& button) const
{
   if (!IsButtonDown(button))
   {
      return false;
   }

   return (GetFrameNumberButtonLastChanged(button) == InputSystem::GetCurrentFrameNumber());
}


//-----------------------------------------------------------------------------------------------
float XboxController::GetTriggerPressure(const XBOX_TRIGGER& trigger) const
{
   return m_triggerPressures[trigger];
}


//-----------------------------------------------------------------------------------------------
const Vector2f& XboxController::GetThumbstickPosition(const XBOX_THUMBSTICK& thumbstick) const
{
   return m_thumbstickPosition[thumbstick];
}


//-----------------------------------------------------------------------------------------------
float XboxController::GetThumbstickRadius(const XBOX_THUMBSTICK& thumbstick) const
{
   return m_thumbstickRadius[thumbstick];
}


//-----------------------------------------------------------------------------------------------
float XboxController::GetThumbstickOrientationDegrees(const XBOX_THUMBSTICK& thumbstick) const
{
   return m_thumbstickOrientationDegrees[thumbstick];
}