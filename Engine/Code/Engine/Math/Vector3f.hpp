#pragma once

#include "Engine/Math/Vector3i.hpp"

//-----------------------------------------------------------------------------------------------
class Vector3f
{
public:
   Vector3f() {}
   Vector3f(float initialX, float initialY, float initialZ);
   Vector3f(const Vector3f& vec);
   Vector3f(const Vector3i& vec);
   ~Vector3f() {}

   Vector3f& operator=(const Vector3f& rhs);
   Vector3f operator+(const Vector3f& rhs) const;
   Vector3f operator+(const Vector3i& rhs) const;
   Vector3f operator-(const Vector3f& rhs) const;
   Vector3f& operator+=(const Vector3f& rhs);
   Vector3f& operator-=(const Vector3f& rhs);
   Vector3f operator*(const float& rhs) const;
   Vector3f operator/(const float& rhs) const;
   Vector3f& operator*=(const float& rhs);
   bool operator==(const Vector3f& rhs) const;
   bool operator!=(const Vector3f& rhs) const;
   Vector3f operator-() const;

   Vector3f GetDirectionWithComponentLengthOne() const;
   float DotProduct(const Vector3f& rhs) const;
   Vector3f CrossProduct(const Vector3f& rhs) const;
   void Normalize();
   Vector3f GetNormalized() const;
   float GetLength() const;
   float GetLengthSquared() const;

   static const Vector3f ZERO;
   static const Vector3f ONE;
   static const Vector3f UP;
   static const Vector3f RIGHT;
   static const Vector3f FORWARD;

   float x;
   float y;
   float z;
};


//-----------------------------------------------------------------------------------------------
inline Vector3f::Vector3f(float initialX, float initialY, float initialZ)
: x(initialX)
, y(initialY)
, z(initialZ) 
{}


inline Vector3f::Vector3f(const Vector3f& vec)
: x(vec.x)
, y(vec.y)
, z(vec.z)
{}


inline Vector3f::Vector3f(const Vector3i& vec)
   : x(float(vec.x))
   , y(float(vec.y))
   , z(float(vec.z))
{}