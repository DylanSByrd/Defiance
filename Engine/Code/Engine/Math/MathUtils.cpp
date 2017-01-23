#include <cmath>
#include <cstdlib>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2f.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/Vector3f.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/EulerAngles.hpp"


//-----------------------------------------------------------------------------------------------
bool DoDiscsOverlap(const Vector2f& center1, float radius1, const Vector2f& center2,
   const float& radius2)
{
   float distSquared = CalcDistSquaredBetweenPoints(center1, center2);
   float radii = radius1 + radius2;
   return (distSquared < (radii * radii));
}


//-----------------------------------------------------------------------------------------------
bool DoAABB2Overlap(const AABB2& box1, const AABB2& box2)
{
   const bool IS_OVERLAPPING = true;

   if (box1.maxs.x < box2.mins.x)
   {
      return !IS_OVERLAPPING;
   }

   if (box1.mins.x > box2.maxs.x)
   {
      return !IS_OVERLAPPING;
   }

   if (box1.maxs.y < box2.mins.y)
   {
      return !IS_OVERLAPPING;
   }

   if (box1.mins.y > box2.maxs.y)
   {
      return !IS_OVERLAPPING;
   }

   return IS_OVERLAPPING;
}


//-----------------------------------------------------------------------------------------------
float CalcDistBetweenPoints(const Vector2f& point1, const Vector2f& point2)
{
   float xDist = point1.x - point2.x;
   float yDist = point1.y - point2.y;
   return sqrt((xDist * xDist) + (yDist * yDist));
}


//-----------------------------------------------------------------------------------------------
float CalcDistSquaredBetweenPoints(const Vector2f& point1, const Vector2f& point2)
{
   float xDist = point1.x - point2.x;
   float yDist = point1.y - point2.y;
   return (xDist * xDist) + (yDist * yDist);
}


//-----------------------------------------------------------------------------------------------
float DegreesToRadians(float degrees)
{
   float radians = degrees * (PI / 180.f);
   return radians;
}


//-----------------------------------------------------------------------------------------------
float RadiansToDegrees(float radians)
{
   float degrees = radians * (180.f / PI);
   return degrees;
}


//-----------------------------------------------------------------------------------------------
float GetRandomFloatBetween(float low, float high)
{
   return low + (rand() / (RAND_MAX / (high - low)));
}


//-----------------------------------------------------------------------------------------------
bool GetRandomTrueOrFalseWithProbability(float probabilityTrue)
{
   return GetRandomFloatBetween(0.f, 1.f) <= probabilityTrue;
}


//-----------------------------------------------------------------------------------------------
bool GetRandomTrueOrFalse()
{
   return rand() % 2 ? true : false;
}


//-----------------------------------------------------------------------------------------------
int GetRandomIntBetweenInclusive(const IntRange& range)
{
   return GetRandomIntBetweenInclusive(range.x, range.y);
}


//-----------------------------------------------------------------------------------------------
int GetRandomIntBetweenInclusive(int low, int high)
{
   return (rand() % ((high + 1) - low) + low);
}


//-----------------------------------------------------------------------------------------------
float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
   float inputScale = (inValue - inStart) / (inEnd - inStart);
   float outputDifference = (outEnd - outStart);
   float outputValue = (inputScale * outputDifference) + outStart;
   return outputValue;
}


//-----------------------------------------------------------------------------------------------
float Clampf(float inValue, float minValue, float maxValue)
{
   if (inValue < minValue)
   {
      return minValue;
   }
   else if (inValue > maxValue)
   {
      return maxValue;
   }
   else
   {
      return inValue;
   }
}


//-----------------------------------------------------------------------------------------------
int Clampi(int inValue, int minValue, int maxValue)
{
   if (inValue < minValue)
   {
      return minValue;
   }
   else if (inValue > maxValue)
   {
      return maxValue;
   }
   else
   {
      return inValue;
   }
}


//-----------------------------------------------------------------------------------------------
float ClampZeroToOnef(float inValue)
{
   if (inValue < 0.f)
   {
      return 0.f;
   }
   else if (inValue > 1.f)
   {
      return 1.f;
   }
   else
   {
      return inValue;
   }
}


//-----------------------------------------------------------------------------------------------
void ConvertCartesianToPolar(float x, float y, float* r, float* thetaRadians)
{
   (*r) = sqrt((x * x) + (y * y));
   (*thetaRadians) = atan2(y, x);
}


//-----------------------------------------------------------------------------------------------
void ConvertPolarToCartesian(float r, float thetaRadians, float* x, float* y)
{
   (*x) = r * cos(thetaRadians);
   (*y) = r * sin(thetaRadians);
}


//-----------------------------------------------------------------------------------------------
bool IsPointInOrOnCircle(const Vector2f& point, const Vector2f& circleCenter, float circleRadius)
{
   float distSquared = CalcDistSquaredBetweenPoints(point, circleCenter);
   return (distSquared <= (circleRadius * circleRadius));
}


//-----------------------------------------------------------------------------------------------
bool IsPointInCircle(const Vector2f& point, const Vector2f& circleCenter, float circleRadius)
{
   float distSquared = CalcDistSquaredBetweenPoints(point, circleCenter);
   return (distSquared < (circleRadius * circleRadius));
}


//-----------------------------------------------------------------------------------------------
float CalcShortestAngularDisplacement(float fromDegrees, float toDegrees)
{
   float angularDisplacement = toDegrees - fromDegrees;
   while (angularDisplacement > 180.f)
   {
      angularDisplacement -= 360.f;
   }

   while (angularDisplacement < -180.f)
   {
      angularDisplacement += 360.f;
   }

   return angularDisplacement;
}


//-----------------------------------------------------------------------------------------------
float CosDegrees(float inputDegrees)
{
   return cos(DegreesToRadians(inputDegrees));
}


//-----------------------------------------------------------------------------------------------
float ACosDegrees(float inputDegrees)
{
   return acos(DegreesToRadians(inputDegrees));
}


//-----------------------------------------------------------------------------------------------
float SinDegrees(float inputDegrees)
{
   return sin(DegreesToRadians(inputDegrees));
}


//-----------------------------------------------------------------------------------------------
float WrapFloatWithinCircularRange(float inValue, float minValue, float maxValue)
{
   float rangeSize = maxValue - minValue;

   while (inValue >= maxValue)
   {
      inValue -= rangeSize;
   }

   while (inValue < minValue)
   {
      inValue += rangeSize;
   }

   return inValue;
}


//-----------------------------------------------------------------------------------------------
int WrapIntWithinCircularRange(int inValue, int minValue, int maxValue)
{
   int rangeSize = maxValue - minValue;

   while (inValue >= maxValue)
   {
      inValue -= rangeSize;
   }

   while (inValue < minValue)
   {
      inValue += rangeSize;
   }

   return inValue;
}


//-----------------------------------------------------------------------------------------------
float SmoothStepUnclamped(float minEdge, float maxEdge, float inValue)
{
   float outValue = ClampZeroToOnef((inValue - minEdge) / (maxEdge - minEdge));
   return (outValue * outValue * (3 - (2 * outValue)));
}


//-----------------------------------------------------------------------------------------------
float SmoothStep(float inputZeroToOne)
{
   float inputSquared = (inputZeroToOne * inputZeroToOne);
   float outVal = (3.f * inputSquared) - (2.f * inputSquared * inputZeroToOne);
   return outVal;
}


//-----------------------------------------------------------------------------------------------
Vector3f Slerp(const Vector3f& start, const Vector3f& end, float time)
{
   float startLength = start.GetLength();
   float endLength = end.GetLength();

   float slerpLength = Lerp(startLength, endLength, time);
   Vector3f slerpUnit = SlerpUnit(start / startLength, end / endLength, time);
   return slerpUnit * slerpLength;
}


//-----------------------------------------------------------------------------------------------
Vector3f SlerpUnit(const Vector3f& start, const Vector3f& end, float time)
{
   float cosAngle = Clampf(start.DotProduct(end), -1.f, 1.f);
   float angle = ACosDegrees(cosAngle);

   if (angle < std::numeric_limits<float>::epsilon())
   {
      return Lerp(start, end, time);
   }
   else
   {
      float positive = SinDegrees(time * angle);
      float negative = SinDegrees((1.f - time) * angle);
      float denominator = SinDegrees(angle);

      return ((start * (negative / denominator)) + (end * (positive / denominator)));
   }
}