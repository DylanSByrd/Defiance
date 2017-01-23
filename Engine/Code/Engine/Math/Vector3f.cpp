#include <cmath>
#include "Engine/Math/Vector3f.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const Vector3f Vector3f::ZERO = Vector3f(0.f, 0.f, 0.f);
STATIC const Vector3f Vector3f::ONE = Vector3f(1.f, 1.f, 1.f);
STATIC const Vector3f Vector3f::UP = Vector3f(0.f, 1.f, 0.f);
STATIC const Vector3f Vector3f::RIGHT = Vector3f(1.f, 0.f, 0.f);
STATIC const Vector3f Vector3f::FORWARD = Vector3f(0.f, 0.f, 1.f);


//-----------------------------------------------------------------------------------------------
Vector3f& Vector3f::operator=(const Vector3f& rhs)
{
   if ((&rhs) == this)
   {
      return *this;
   }

   x = rhs.x;
   y = rhs.y;
   z = rhs.z;
   return *this;
}


//-----------------------------------------------------------------------------------------------
Vector3f Vector3f::operator+(const Vector3f& rhs) const
{
   return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
}


//-----------------------------------------------------------------------------------------------
Vector3f Vector3f::operator+(const Vector3i& rhs) const
{
   return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
}


//-----------------------------------------------------------------------------------------------
Vector3f Vector3f::operator-(const Vector3f& rhs) const
{
   return Vector3f(x - rhs.x, y - rhs.y, z - rhs.z);
}


//-----------------------------------------------------------------------------------------------
Vector3f& Vector3f::operator+=(const Vector3f& rhs)
{
   x += rhs.x;
   y += rhs.y;
   z += rhs.z;
   return *this;
}


//-----------------------------------------------------------------------------------------------
Vector3f& Vector3f::operator-=(const Vector3f& rhs)
{
   x -= rhs.x;
   y -= rhs.y;
   z -= rhs.z;
   return *this;
}


//-----------------------------------------------------------------------------------------------
Vector3f Vector3f::operator*(const float& rhs) const
{
   return Vector3f(x * rhs, y * rhs, z * rhs);
}


//-----------------------------------------------------------------------------------------------
Vector3f Vector3f::operator/(const float& rhs) const
{
   return Vector3f(x / rhs, y / rhs, z / rhs);
}


//-----------------------------------------------------------------------------------------------
Vector3f& Vector3f::operator*=(const float& rhs)
{
   x *= rhs;
   y *= rhs;
   z *= rhs;
   return *this;
}


//-----------------------------------------------------------------------------------------------
bool Vector3f::operator==(const Vector3f& rhs) const
{
   if (x == rhs.x && y == rhs.y && z == rhs.z)
   {
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
bool Vector3f::operator!=(const Vector3f& rhs) const
{
   return !(*this == rhs);
}


//-----------------------------------------------------------------------------------------------
Vector3f Vector3f::operator-() const
{
   Vector3f negativeThis(-x, -y, -z);
   return negativeThis;
}


//-----------------------------------------------------------------------------------------------
Vector3f Vector3f::GetDirectionWithComponentLengthOne() const
{
   Vector3f result = *this;
   result.x /= result.x;
   result.y /= result.y;
   result.z /= result.z;
   return result;
}


//-----------------------------------------------------------------------------------------------
float Vector3f::DotProduct(const Vector3f& rhs) const
{
   return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
}


//-----------------------------------------------------------------------------------------------
Vector3f Vector3f::CrossProduct(const Vector3f& rhs) const
{
   Vector3f result;
   result.x = (y * rhs.z) - (z * rhs.y);
   result.y = (z * rhs.x) - (x * rhs.z);
   result.z = (x * rhs.y) - (y * rhs.x);

   return result;
}


//-----------------------------------------------------------------------------------------------
void Vector3f::Normalize()
{
   const float length = GetLength();
   if (length != 0)
   {
      x = x / length;
      y = y / length;
      z = z / length;
   }
}


//-----------------------------------------------------------------------------------------------
Vector3f Vector3f::GetNormalized() const
{
   Vector3f copy = *this;

   copy.Normalize();
   return copy;
}


//-----------------------------------------------------------------------------------------------
float Vector3f::GetLength() const
{
   return sqrt((x * x) + (y * y) + (z * z));
}


//-----------------------------------------------------------------------------------------------
float Vector3f::GetLengthSquared() const
{
   return ((x * x) + (y * y) + (z * z));
}