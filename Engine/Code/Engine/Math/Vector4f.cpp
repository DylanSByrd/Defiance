#include "Engine/Math/Vector4f.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
float Vector4f::DotProduct(const Vector4f& rhs) const
{
   return (x *rhs.x) + (y * rhs.y) + (z * rhs.z) + (w * rhs.w);
}


//-----------------------------------------------------------------------------------------------
float& Vector4f::operator[](int index)
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
void Vector4f::operator/=(float divisor)
{
   ASSERT_OR_DIE(divisor != 0, "ERROR: Trying to divide vec4 by 0!");

   x /= divisor;
   y /= divisor;
   z /= divisor;
   w /= divisor;
}


//-----------------------------------------------------------------------------------------------
bool Vector4f::operator==(const Vector4f& rhs) const
{
   return (x == rhs.x
      && y == rhs.y
      && z == rhs.z
      && w == rhs.w);
}