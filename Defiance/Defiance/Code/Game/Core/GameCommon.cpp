#include "Engine/Math/MathUtils.hpp"
#include "Game/Core/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
TileDirection GetRandomTileDirection()
{
   int directionIndex = GetRandomIntBetweenInclusive(0, 7);
   return (TileDirection)directionIndex;
}