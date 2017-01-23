#pragma once

//-----------------------------------------------------------------------------------------------
enum PrimitiveType;


//-----------------------------------------------------------------------------------------------
struct DrawInstruction
{
   PrimitiveType primitive;
   unsigned int startIndex;
   unsigned int count;
   bool isUsingIndexBuffer;
};