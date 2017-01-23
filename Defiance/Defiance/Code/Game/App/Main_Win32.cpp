#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Time/Time.hpp"
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Audio/TheAudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Debug/Console.hpp"
#include "Game/App/TheApp.hpp"
#include "Game/Core/TheGame.hpp"
#include <gl/Gl.h>


//-----------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
HWND g_hWnd = nullptr;
HDC g_displayDeviceContext = nullptr;
HGLRC g_openGLRenderingContext = nullptr;


//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
   if (g_theApp == nullptr)
   {
      return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
   }

   unsigned char asKey = (unsigned char)wParam;
   switch (wmMessageCode)
   {
   case WM_CLOSE:
   case WM_DESTROY:
   case WM_QUIT:
      TheApp::QuitApplication();
      return 0;

   case WM_KEYDOWN:
      g_theInputSystem->SetKeyDown(asKey);
      break;

   case WM_KEYUP:
      g_theInputSystem->SetKeyUp(asKey);
      break;

   case WM_CHAR:
      if (g_theConsole->GetDisplayMode() != Console::NO_DISPLAY)
      {
         g_theConsole->PipeKey(asKey);
      }
      break;

   case WM_SETFOCUS:
      g_theInputSystem->SetWindowHasFocus(true);
      break;

   case WM_KILLFOCUS:
      g_theInputSystem->SetWindowHasFocus(false);
      break;

   case WM_LBUTTONDOWN:
      g_theInputSystem->SetMouseButtonDown(MouseHandler::LEFT_BUTTON);
      break;

   case WM_LBUTTONUP:
      g_theInputSystem->SetMouseButtonUp(MouseHandler::LEFT_BUTTON);
      break;

   case WM_RBUTTONDOWN:
      g_theInputSystem->SetMouseButtonDown(MouseHandler::RIGHT_BUTTON);
      break;

   case WM_RBUTTONUP:
      g_theInputSystem->SetMouseButtonUp(MouseHandler::RIGHT_BUTTON);
      break;

   case WM_MOUSEWHEEL:
      g_theInputSystem->SetMouseWheelDelta(GET_WHEEL_DELTA_WPARAM(wParam));
      break;
   }

   return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}


//-----------------------------------------------------------------------------------------------
void CreateOpenGLWindow(HINSTANCE applicationInstanceHandle)
{
   // Define a window class
   WNDCLASSEX windowClassDescription;
   memset(&windowClassDescription, 0, sizeof(windowClassDescription));
   windowClassDescription.cbSize = sizeof(windowClassDescription);
   windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
   windowClassDescription.lpfnWndProc = WNDPROC(WindowsMessageHandlingProcedure); // Assign a win32 message-handling function
   windowClassDescription.hInstance = GetModuleHandle(NULL);
   windowClassDescription.hIcon = NULL;
   windowClassDescription.hCursor = NULL;
   windowClassDescription.lpszClassName = TEXT("Simple Window Class");
   RegisterClassEx(&windowClassDescription);

   const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
   const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

   RECT desktopRect;
   HWND desktopWindowHandle = GetDesktopWindow();
   GetClientRect(desktopWindowHandle, &desktopRect);

   RECT windowRect = { TheApp::OFFSET_FROM_WINDOWS_DESKTOP, TheApp::OFFSET_FROM_WINDOWS_DESKTOP, TheApp::OFFSET_FROM_WINDOWS_DESKTOP + TheApp::WINDOW_PHYSICAL_WIDTH, TheApp::OFFSET_FROM_WINDOWS_DESKTOP + TheApp::WINDOW_PHYSICAL_HEIGHT };
   AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

   WCHAR windowTitle[1024];
   MultiByteToWideChar(GetACP(), 0, TheApp::APP_NAME, -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
   g_hWnd = CreateWindowEx(
      windowStyleExFlags,
      windowClassDescription.lpszClassName,
      windowTitle,
      windowStyleFlags,
      windowRect.left,
      windowRect.top,
      windowRect.right - windowRect.left,
      windowRect.bottom - windowRect.top,
      NULL,
      NULL,
      applicationInstanceHandle,
      NULL);

   ShowWindow(g_hWnd, SW_SHOW);
   SetForegroundWindow(g_hWnd);
   SetFocus(g_hWnd);

   g_displayDeviceContext = GetDC(g_hWnd);

   HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
   SetCursor(cursor);

   PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
   memset(&pixelFormatDescriptor, 0, sizeof(pixelFormatDescriptor));
   pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
   pixelFormatDescriptor.nVersion = 1;
   pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
   pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
   pixelFormatDescriptor.cColorBits = 24;
   pixelFormatDescriptor.cDepthBits = 24;
   pixelFormatDescriptor.cAccumBits = 0;
   pixelFormatDescriptor.cStencilBits = 8;

   int pixelFormatCode = ChoosePixelFormat(g_displayDeviceContext, &pixelFormatDescriptor);
   SetPixelFormat(g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor);
   g_openGLRenderingContext = wglCreateContext(g_displayDeviceContext);
   wglMakeCurrent(g_displayDeviceContext, g_openGLRenderingContext);
}


//-----------------------------------------------------------------------------------------------
void RunMessagePump()
{
   MSG queuedMessage;
   for (;;)
   {
      const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
      if (!wasMessagePresent)
      {
         break;
      }

      TranslateMessage(&queuedMessage);
      DispatchMessage(&queuedMessage);
   }
}


//-----------------------------------------------------------------------------------------------
void Update()
{
   g_theApp->Update();
}


//-----------------------------------------------------------------------------------------------
void Render()
{
   g_theApp->Render();
   SwapBuffers(g_displayDeviceContext);
}


//-----------------------------------------------------------------------------------------------
void RunFrame()
{
   RunMessagePump();
   Update();
   Render();
}


//-----------------------------------------------------------------------------------------------
void Initialize(HINSTANCE applicationInstanceHandle)
{
   CreateOpenGLWindow(applicationInstanceHandle);
   g_theApp = new TheApp();
   g_theApp->Init();
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
   g_theApp->Shutdown();
   delete g_theApp;
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{
   // #TODO: REMOVE THIS MEMORYLEAK CHECKER. 
   // Or not. It's kinda useful and doesn't show up in release
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

   UNUSED(commandLineString);
   Initialize(applicationInstanceHandle);
   while (!TheApp::IsQuitting())
   {
      RunFrame();
   }

   Shutdown();
   return 0;
}