#pragma once

#include "Vector3f.hpp"


// TODO - FILL OUT
class Vector4f
{
public:
   Vector4f() : x(0.f), y(0.f), z(0.f), w(0.f) {}
   Vector4f(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ), w(1.0f) {}
   Vector4f(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {}
   Vector4f(const Vector3f& xyz, float inW) : x(xyz.x), y(xyz.y), z(xyz.z), w(inW) {}
   ~Vector4f() {}

   float DotProduct(const Vector4f& rhs) const;
   Vector3f xyz() const { return Vector3f(x, y, z); }

   float& operator[](int index);
   void operator/=(float divisor);
   bool operator==(const Vector4f& rhs) const;


   float x;
   float y;
   float z;
   float w;
};