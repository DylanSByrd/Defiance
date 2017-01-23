#pragma once

#include <string>
#include "Engine/Core/ByteUtils.hpp"


//-----------------------------------------------------------------------------------------------
class IBinaryReader
{
public:
   IBinaryReader() : m_mode(LITTLE_ENDIAN) {}
   virtual size_t ReadBytes(void* destination, const size_t numberOfBytesToRead) = 0;
   bool ReadString(std::string* outString);
   bool ReadInt(int* outInt);
   bool ReadFloat(float* outFloat);

   template <typename TypeToRead>
   bool Read(TypeToRead* outBuffer);

   void SetEndianMode(EndianMode mode) { m_mode = mode; }
   void SetEndianModeToLocalMode() { m_mode = GetLocalEndianMode(); }

protected:
   EndianMode m_mode;
};


//-----------------------------------------------------------------------------------------------
template <typename TypeToRead>
bool IBinaryReader::Read(TypeToRead* outBuffer)
{
   bool isReadSuccess = true;
   bool readResult = ReadBytes(outBuffer, sizeof(TypeToRead)) == sizeof(TypeToRead);

   if (!readResult)
   {
      return !isReadSuccess;
   }
   
   EndianMode localEndianMode = GetLocalEndianMode();
   if (localEndianMode != m_mode)
   {
      ByteSwap(outBuffer, sizeof(TypeToRead));
   }

   return isReadSuccess;
}