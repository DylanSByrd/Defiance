#include "Engine/Math/AABB2.hpp"


//-----------------------------------------------------------------------------------------------
AABB2::AABB2(const Vector2i& mins, const Vector2i& maxs)
: mins(Vector2f(float(mins.x), float(mins.y)))
, maxs(Vector2f(float(maxs.x), float(maxs.y)))
{
}