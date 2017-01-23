#include <cmath>
#include "Engine/Math/Vector3i.hpp"
#include "Engine/Math/Vector3f.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const Vector3i Vector3i::ZERO = Vector3i(0, 0, 0);


//-----------------------------------------------------------------------------------------------
Vector3i& Vector3i::operator=(const Vector3i& rhs)
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
Vector3i Vector3i::operator+(const Vector3i& rhs) const
{
   return Vector3i(x + rhs.x, y + rhs.y, z + rhs.z);
}


//-----------------------------------------------------------------------------------------------
Vector3f Vector3i::operator+(const Vector3f& rhs) const
{
   return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
}


//-----------------------------------------------------------------------------------------------
Vector3i Vector3i::operator-(const Vector3i& rhs) const
{
   return Vector3i(x - rhs.x, y - rhs.y, z - rhs.z);
}


//-----------------------------------------------------------------------------------------------
Vector3i& Vector3i::operator+=(const Vector3i& rhs)
{
   x += rhs.x;
   y += rhs.y;
   z += rhs.z;
   return *this;
}


//-----------------------------------------------------------------------------------------------
Vector3i& Vector3i::operator-=(const Vector3i& rhs)
{
   x -= rhs.x;
   y -= rhs.y;
   z -= rhs.z;
   return *this;
}


//-----------------------------------------------------------------------------------------------
bool Vector3i::operator!=(const Vector3i& rhs) const
{
   return !(*this == rhs);
}


//-----------------------------------------------------------------------------------------------
bool Vector3i::operator==(const Vector3i& rhs) const
{
   if (x == rhs.x && y == rhs.y && z == rhs.z)
   {
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
float Vector3i::GetLength() const
{
   return float(sqrt((x * x) + (y * y) + (z * z)));
}