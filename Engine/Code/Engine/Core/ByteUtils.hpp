#pragma once


//-----------------------------------------------------------------------------------------------
#define BIT(x) (1 << (x))
void SetBit(unsigned char& bitFlags, unsigned char bitMask);
void ClearBit(unsigned char& bitFlags, unsigned char bitMask);
bool IsPowerOfTwo(int x);
bool IsBitSet(unsigned char bitFlags, unsigned char bitMask);


//-----------------------------------------------------------------------------------------------
typedef unsigned char byte_t;


//-----------------------------------------------------------------------------------------------
enum EndianMode
{
   LITTLE_ENDIAN,
   BIG_ENDIAN,
   NUM_ENDIAN_MODES,
};
EndianMode GetLocalEndianMode();


//-----------------------------------------------------------------------------------------------
void ByteSwap(void* data, const size_t dataSize);