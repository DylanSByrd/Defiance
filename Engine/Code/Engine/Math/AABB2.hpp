#pragma once

#include "Engine/Math/Vector2f.hpp"
#include "Engine/Math/Vector2i.hpp"


//-----------------------------------------------------------------------------------------------
class AABB2
{
public:
   AABB2() {}
   AABB2(const Vector2f& mins, const Vector2f& maxs) : mins(mins), maxs(maxs) {}
   AABB2(const Vector2i& mins, const Vector2i& maxs);
   AABB2(const AABB2& aabb2) : mins(aabb2.mins), maxs(aabb2.maxs) {}
   ~AABB2() {}

   Vector2f mins;
   Vector2f maxs;
};