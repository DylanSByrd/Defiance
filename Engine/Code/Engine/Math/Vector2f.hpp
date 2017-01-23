#pragma once

#include "Engine/Math/Vector2i.hpp"

//-----------------------------------------------------------------------------------------------
class Vector2i;


//-----------------------------------------------------------------------------------------------
class Vector2f
{
public:
   Vector2f() {}
   Vector2f(float initialX, float initialY) : x(initialX), y(initialY) {}
   Vector2f(const Vector2f& vec) : x(vec.x), y(vec.y) {}
   Vector2f(const Vector2i& vec) : x(float(vec.x)), y(float(vec.y)) {}
   ~Vector2f() {}

   Vector2f& operator=(const Vector2f& rhs);
   Vector2f operator+(const Vector2f& rhs) const;
   Vector2f operator-(const Vector2f& rhs) const;
   Vector2f& operator+=(const Vector2f& rhs);
   Vector2f& operator-=(const Vector2f& rhs);
   Vector2f operator*(const float& rhs) const;
   Vector2f operator/(const float& rhs) const;
   Vector2f& operator*=(const float& rhs);
   bool operator==(const Vector2f& rhs) const;
   bool operator!=(const Vector2f& rhs) const;
   Vector2f operator-() const;
   Vector2f& operator=(const Vector2i& rhs);

   void Normalize();
   float GetLength() const;
   float GetLengthSquared() const;

   static float DotProduct(const Vector2f& lhs, const Vector2f& rhs);
   
   static const Vector2f ZERO;
   static const Vector2f ONE;

   float x;
   float y;
};