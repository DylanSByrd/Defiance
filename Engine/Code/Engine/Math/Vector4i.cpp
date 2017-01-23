#include "Engine/Math/Vector4i.hpp"
#include "Engine/Math/Vector4f.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const Vector4i Vector4i::ZERO = Vector4i(0, 0, 0, 0);
STATIC const Vector4i Vector4i::ONE = Vector4i(1, 1, 1, 1);


//-----------------------------------------------------------------------------------------------
Vector4i::Vector4i(int initialX, int initialY, int initialZ, int initialW)
   : x(initialX)
   , y(initialY)
   , z(initialZ)
   , w(initialW)
{
}


//-----------------------------------------------------------------------------------------------
Vector4i::Vector4i(const Vector4i& vec)
   : x(vec.x)
   , y(vec.y)
   , z(vec.z)
   , w(vec.w)
{
}


//-----------------------------------------------------------------------------------------------
Vector4i::Vector4i(const Vector4f& vec)
   : x((int)floor(vec.x))
   , y((int)floor(vec.y))
   , z((int)floor(vec.z))
   , w((int)floor(vec.w))
{
}


//-----------------------------------------------------------------------------------------------
int& Vector4i::operator[](int index)
{
   switch (index)
   {
   case 0:
   {
      return x;
   }

   case 1:
   {
      return y;
   }

   case 2:
   {
      return z;
   }

   case 3:
   {
      return w;
   }
   }

   ERROR_AND_DIE("ERROR: Invalid vertex component index!");
}


//-----------------------------------------------------------------------------------------------
bool Vector4i::operator==(const Vector4i& rhs) const
{
   return (x == rhs.x
      && y == rhs.y
      && z == rhs.z
      && w == rhs.w);
}