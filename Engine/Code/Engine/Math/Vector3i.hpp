#pragma once


//-----------------------------------------------------------------------------------------------
class Vector3f;


//-----------------------------------------------------------------------------------------------
class Vector3i
{
public:
   Vector3i() {}
   Vector3i(int initialX, int initialY, int initialZ);
   Vector3i(const Vector3i& vec);
   ~Vector3i() {}

   Vector3i& operator=(const Vector3i& rhs);
   Vector3i operator+(const Vector3i& rhs) const;
   Vector3f operator+(const Vector3f& rhs) const;
   Vector3i operator-(const Vector3i& rhs) const;
   Vector3i& operator+=(const Vector3i& rhs);
   Vector3i& operator-=(const Vector3i& rhs);
   bool operator==(const Vector3i& rhs) const;
   bool operator!=(const Vector3i& rhs) const;

   float GetLength() const;

   static const Vector3i ZERO;

   int x;
   int y;
   int z;
};

inline Vector3i::Vector3i(int initialX, int initialY, int initialZ)
   : x(initialX)
   , y(initialY)
   , z(initialZ)
{}

inline Vector3i::Vector3i(const Vector3i& vec)
   : x(vec.x)
   , y(vec.y)
   , z(vec.z)
{}