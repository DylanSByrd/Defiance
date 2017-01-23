#include <cmath>
#include "Engine/Math/Vector2f.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const Vector2f Vector2f::ZERO = Vector2f(0.f, 0.f);
STATIC const Vector2f Vector2f::ONE = Vector2f(1.f, 1.f);


//-----------------------------------------------------------------------------------------------
Vector2f& Vector2f::operator=(const Vector2f& rhs)
{
   if ((&rhs) == this)
   {
      return *this;
   }

   x = rhs.x;
   y = rhs.y;
   return *this;
}


//-----------------------------------------------------------------------------------------------
Vector2f Vector2f::operator+(const Vector2f& rhs) const
{
   return Vector2f(x + rhs.x, y + rhs.y);
}


//-----------------------------------------------------------------------------------------------
Vector2f Vector2f::operator-(const Vector2f& rhs) const
{
   return Vector2f(x - rhs.x, y - rhs.y);
}


//-----------------------------------------------------------------------------------------------
Vector2f& Vector2f::operator+=(const Vector2f& rhs)
{
   x += rhs.x;
   y += rhs.y;
   return *this;
}


//-----------------------------------------------------------------------------------------------
Vector2f& Vector2f::operator-=(const Vector2f& rhs)
{
   x -= rhs.x;
   y -= rhs.y;
   return *this;
}


//-----------------------------------------------------------------------------------------------
Vector2f Vector2f::operator*(const float& rhs) const
{
   return Vector2f(x * rhs, y * rhs);
}


//-----------------------------------------------------------------------------------------------
Vector2f Vector2f::operator/(const float& rhs) const
{
   return Vector2f(x / rhs, y / rhs);
}


//-----------------------------------------------------------------------------------------------
Vector2f& Vector2f::operator*=(const float& rhs)
{
   x *= rhs;
   y *= rhs;
   return *this;
}


//-----------------------------------------------------------------------------------------------
bool Vector2f::operator==(const Vector2f& rhs) const
{
   if (x == rhs.x && y == rhs.y)
   {
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
bool Vector2f::operator!=(const Vector2f& rhs) const
{
   return !(*this == rhs);
}


//-----------------------------------------------------------------------------------------------
Vector2f Vector2f::operator-() const
{
   Vector2f negativeThis(-x, -y);
   return negativeThis;
}


//-----------------------------------------------------------------------------------------------
Vector2f& Vector2f::operator=(const Vector2i& rhs)
{
   x = float(rhs.x);
   y = float(rhs.y);
   return *this;
}


//-----------------------------------------------------------------------------------------------
STATIC float Vector2f::DotProduct(const Vector2f& lhs, const Vector2f& rhs)
{
   return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}


//-----------------------------------------------------------------------------------------------
void Vector2f::Normalize()
{
   const float length = GetLength();
   if (length != 0)
   {
      x = x / length;
      y = y / length;
   }
}


//-----------------------------------------------------------------------------------------------
float Vector2f::GetLength() const
{
   return sqrt((x * x) + (y * y));
}


//-----------------------------------------------------------------------------------------------
float Vector2f::GetLengthSquared() const
{
   return (x * x) + (y * y);
}