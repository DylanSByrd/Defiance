#pragma once


//-----------------------------------------------------------------------------------------------
class Vector4f;


//-----------------------------------------------------------------------------------------------
class Vector4i
{
public:
   Vector4i() {}
   Vector4i(int initialX, int initialY, int initialZ, int initialW);
   Vector4i(const Vector4i& vec);
   Vector4i(const Vector4f& vec);

   //#TODO - as needed
   //Vector4i& operator=(const Vector4i& rhs);
   int& operator[](int index);
   Vector4i operator+(const Vector4i& rhs) const;
   Vector4i operator-(const Vector4i& rhs) const;
   Vector4i& operator+=(const Vector4i& rhs);
   Vector4i& operator-=(const Vector4i& rhs);
   bool operator==(const Vector4i& rhs) const;
   bool operator!=(const Vector4i& rhs) const { return !operator==(rhs); }
   bool operator<(const Vector4i& rhs) const;

   static const Vector4i ZERO;
   static const Vector4i ONE;

   int x;
   int y;
   int z;
   int w;
};