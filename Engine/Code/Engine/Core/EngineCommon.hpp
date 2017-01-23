#pragma once

// Mem leak checker
//#define _CRTDBG_MAP_ALLOC #ifdef _DEBUG
//#ifndef DBG_NEW
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//#define new DBG_NEW
//#endif #endif // _DEBUG


#define STATIC


//-----------------------------------------------------------------------------------------------
#include <string>
#include "Engine/Math/Vector2f.hpp"


//-----------------------------------------------------------------------------------------------
class AABB2;
class Vector2i;
class ConsoleCommandArgs;


//-----------------------------------------------------------------------------------------------
typedef Vector2i SpriteCoords;
typedef Vector2i ScreenCoords;
typedef Vector2f TexCoords;
typedef AABB2 TexBounds;
typedef int SpriteIndex;