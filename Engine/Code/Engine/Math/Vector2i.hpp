#pragma once

#include <string>


//-----------------------------------------------------------------------------------------------
class Vector2f;


//-----------------------------------------------------------------------------------------------
class Vector2i
{
public:
   Vector2i() {}
   Vector2i(int initialX, int initialY) : x(initialX), y(initialY) {}
   Vector2i(const Vector2i& vec) : x(vec.x), y(vec.y) {}
   Vector2i(const Vector2f& vec);

   Vector2i& operator=(const Vector2i& rhs);
   Vector2i operator+(const Vector2i& rhs) const;
   Vector2i operator-(const Vector2i& rhs) const;
   Vector2i& operator+=(const Vector2i& rhs);
   Vector2i& operator-=(const Vector2i& rhs);
   bool operator==(const Vector2i& rhs) const;
   bool operator!=(const Vector2i& rhs) const;
   bool operator<(const Vector2i& rhs) const;

   float GetLength() const;
   static float GetDistanceBetween(const Vector2i& lhs, const Vector2i& rhs);
   int GetManhattanDistanceToVector(const Vector2i& rhs) const;
   std::string ToString() const;

   static const Vector2i ZERO;
   static const Vector2i ONE;

   int x;
   int y;
};