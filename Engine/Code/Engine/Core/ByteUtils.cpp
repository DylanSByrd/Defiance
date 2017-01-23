#include <cstdint>
#include <algorithm>
#include "Engine/Core/ByteUtils.hpp"


//-----------------------------------------------------------------------------------------------
void SetBit(unsigned char& bitFlags, unsigned char bitMask)
{
   bitFlags |= bitMask;
}


//-----------------------------------------------------------------------------------------------
void ClearBit(unsigned char& bitFlags, unsigned char bitMask)
{
   bitFlags &= ~bitMask;
}


//-----------------------------------------------------------------------------------------------
bool IsPowerOfTwo(int x)
{
   return ((x & (x - 1)) == 0);
}


//-----------------------------------------------------------------------------------------------
bool IsBitSet(unsigned char bitFlags, unsigned char bitMask)
{
   return ((bitFlags & bitMask) != 0);
}


//-----------------------------------------------------------------------------------------------
EndianMode GetLocalEndianMode()
{
   union
   {
      byte_t byteData[4];
      uint32_t unsignedIntData;
   } data;

   data.unsignedIntData = 0x04030201;
   return (data.byteData[0] == 0x01) ? LITTLE_ENDIAN : BIG_ENDIAN;
}


//-----------------------------------------------------------------------------------------------
void ByteSwap(void* data, const size_t dataSize)
{
   byte_t* dataAsBytes = (byte_t*)data;
   size_t lastIndex = dataSize - 1;
   for (size_t byteIndex = 0; byteIndex < dataSize / 2; ++byteIndex)
   {
      std::swap(dataAsBytes[byteIndex], dataAsBytes[lastIndex - byteIndex]);
   }
}


