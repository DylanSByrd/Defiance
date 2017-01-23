#pragma once


//-----------------------------------------------------------------------------------------------
class IntRange
{
public:
   IntRange() {}
   IntRange(int newX, int newY) : x(newX), y(newY) {}

   static const IntRange ZERO;

   // #TODO - min and max
   int x;
   int y;
};