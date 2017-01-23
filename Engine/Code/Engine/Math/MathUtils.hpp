#pragma once

#include <vector>


//-----------------------------------------------------------------------------------------------
class Vector2i;
class IntRange;
class Vector2f;
class Vector3f;
class AABB2;
class EulerAngles;


//-----------------------------------------------------------------------------------------------
const float PI = 3.1415926535897932384626433832795f;
const float TAU = PI * 2.f;


//-----------------------------------------------------------------------------------------------
bool DoDiscsOverlap(const Vector2f& center1, float radius1, const Vector2f& center2,
   const float& radius2);


//-----------------------------------------------------------------------------------------------
bool DoAABB2Overlap(const AABB2& box1, const AABB2& box2);


//-----------------------------------------------------------------------------------------------
float CalcDistBetweenPoints(const Vector2f& point1, const Vector2f& point2);


//-----------------------------------------------------------------------------------------------
float CalcDistSquaredBetweenPoints(const Vector2f& point1, const Vector2f& point2);


//-----------------------------------------------------------------------------------------------
float DegreesToRadians(float degrees);


//-----------------------------------------------------------------------------------------------
float RadiansToDegrees(float radians);


//-----------------------------------------------------------------------------------------------
float GetRandomFloatBetween(float low, float high);


//-----------------------------------------------------------------------------------------------
bool GetRandomTrueOrFalseWithProbability(float probabilityTrue);


//-----------------------------------------------------------------------------------------------
bool GetRandomTrueOrFalse();


//-----------------------------------------------------------------------------------------------
int GetRandomIntBetweenInclusive(int low, int high);
int GetRandomIntBetweenInclusive(const IntRange& range);


//-----------------------------------------------------------------------------------------------
float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd);


//-----------------------------------------------------------------------------------------------
float Clampf(float inValue, float minValue, float maxValue);
int Clampi(int inValue, int minValue, int maxValue);


//-----------------------------------------------------------------------------------------------
float ClampZeroToOnef(float inValue);


//-----------------------------------------------------------------------------------------------
void ConvertCartesianToPolar(float x, float y, float* r, float* thetaRadians);


//-----------------------------------------------------------------------------------------------
void ConvertPolarToCartesian(float r, float thetaRadians, float* x, float* y);


//-----------------------------------------------------------------------------------------------
bool IsPointInOrOnCircle(const Vector2f& point, const Vector2f& circleCenter, float circleRadius);


//-----------------------------------------------------------------------------------------------
bool IsPointInCircle(const Vector2f& point, const Vector2f& circleCenter, float circleRadius);


//-----------------------------------------------------------------------------------------------
float CalcShortestAngularDisplacement(float fromDegrees, float toDegrees);


//-----------------------------------------------------------------------------------------------
float CosDegrees(float inputDegrees);
float ACosDegrees(float inputDegrees);
float SinDegrees(float inputDegrees);


//-----------------------------------------------------------------------------------------------
float WrapFloatWithinCircularRange(float inValue, float minValue, float maxValue);


//-----------------------------------------------------------------------------------------------
int WrapIntWithinCircularRange(int inValue, int minValue, int maxValue);


//-----------------------------------------------------------------------------------------------
float SmoothStepUnclamped(float minEdge, float maxEdge, float inValue);


//-----------------------------------------------------------------------------------------------
float SmoothStep(float inputZeroToOne);


//-----------------------------------------------------------------------------------------------
Vector3f Slerp(const Vector3f& start, const Vector3f& end, float time);
Vector3f SlerpUnit(const Vector3f& start, const Vector3f& end, float time);


//-----------------------------------------------------------------------------------------------
template <typename LerpType>
LerpType Lerp(LerpType start, LerpType end, float time)
{
   return start * (1.0f - time) + end * time;
}


//-----------------------------------------------------------------------------------------------
template <typename CollectionType>
int GetRandomIndexFromCollection(const std::vector<CollectionType>& collection)
{
   return GetRandomIntBetweenInclusive(0, collection.size() - 1);
}

template <typename CollectionType>
const CollectionType& GetRandomFromCollection(const std::vector<CollectionType>& collection)
{
   return collection[GetRandomIntBetweenInclusive(0, collection.size() - 1)];
}