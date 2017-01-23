#pragma once

#include "Engine/Math/Vector3f.hpp"
#include "Engine/Math/Vector3i.hpp"


//-----------------------------------------------------------------------------------------------
class AABB3
{
public:
   AABB3() {}
   AABB3(const Vector3f& mins, const Vector3f& maxs) : mins(mins), maxs(maxs) {}
   AABB3(const Vector3i& mins, const Vector3i& maxs) : mins(mins), maxs(maxs) {}
   AABB3(const AABB3& aabb3) : mins(aabb3.mins), maxs(aabb3.maxs) {}
   ~AABB3() {}

   Vector3f mins;
   Vector3f maxs;
};