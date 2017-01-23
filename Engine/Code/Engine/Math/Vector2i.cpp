#include <cmath>
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const Vector2i Vector2i::ZERO = Vector2i(0, 0);
STATIC const Vector2i Vector2i::ONE = Vector2i(1, 1);


//-----------------------------------------------------------------------------------------------
Vector2i::Vector2i(const Vector2f& vec)
   : x((int)floor(vec.x))
   , y((int)floor(vec.y))
{}


//-----------------------------------------------------------------------------------------------
Vector2i& Vector2i::operator=(const Vector2i& rhs)
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
Vector2i Vector2i::operator+(const Vector2i& rhs) const
{
   return Vector2i(x + rhs.x, y + rhs.y);
}


//-----------------------------------------------------------------------------------------------
Vector2i Vector2i::operator-(const Vector2i& rhs) const
{
   return Vector2i(x - rhs.x, y - rhs.y);
}


//-----------------------------------------------------------------------------------------------
Vector2i& Vector2i::operator+=(const Vector2i& rhs)
{
   x += rhs.x;
   y += rhs.y;
   return *this;
}


//-----------------------------------------------------------------------------------------------
Vector2i& Vector2i::operator-=(const Vector2i& rhs)
{
   x -= rhs.x;
   y -= rhs.y;
   return *this;
}


//-----------------------------------------------------------------------------------------------
bool Vector2i::operator!=(const Vector2i& rhs) const
{
   return !(*this == rhs);
}


//-----------------------------------------------------------------------------------------------
bool Vector2i::operator==(const Vector2i& rhs) const
{
   if (x == rhs.x && y == rhs.y)
   {
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
bool Vector2i::operator<(const Vector2i& rhs) const
{
   if (y < rhs.y)
   {
      return true;
   }
   
   if (rhs.y < y)
   {
      return false;
   }

   return (x < rhs.x);
}


//-----------------------------------------------------------------------------------------------
float Vector2i::GetLength() const
{
   return float(sqrt((x * x) + (y * y)));
}


//-----------------------------------------------------------------------------------------------
STATIC float Vector2i::GetDistanceBetween(const Vector2i& lhs, const Vector2i& rhs)
{
   Vector2i magnitude = rhs - lhs;
   return magnitude.GetLength();
}


//-----------------------------------------------------------------------------------------------
int Vector2i::GetManhattanDistanceToVector(const Vector2i& rhs) const
{
   return std::abs(x - rhs.x) + std::abs(y - rhs.y);
}


//-----------------------------------------------------------------------------------------------
std::string Vector2i::ToString() const
{
   std::string asString;
   char buf[50];
   _itoa_s(x, buf, 10);
   asString += buf;
   asString.push_back(',');
   _itoa_s(y, buf, 10);
   asString += buf;

   return asString;
}