#pragma once

#include <string>
#include <vector>
#include "Engine/Core/ByteUtils.hpp"


//-----------------------------------------------------------------------------------------------
class IBinaryWriter
{
public:
   IBinaryWriter() : m_mode(LITTLE_ENDIAN) {}
   virtual size_t WriteBytes(const void* source, const size_t numberOfBytesToWrite) = 0;
   bool WriteString(const std::string& stringToWrite);
   bool WriteInt(int intToWrite);
   bool WriteFloat(float floatToWrite);

   template <typename TypeToWrite>
   bool Write(const TypeToWrite& value);

   void SetEndianMode(EndianMode mode) { m_mode = mode; }
   void SetEndianModeToLocalMode() { m_mode = GetLocalEndianMode(); }

protected:
   EndianMode m_mode;
};


//-----------------------------------------------------------------------------------------------
template <typename TypeToWrite>
bool IBinaryWriter::Write(const TypeToWrite& value)
{
   TypeToWrite copy = value;
   EndianMode localEndianMode = GetLocalEndianMode();
   if (localEndianMode != m_mode)
   {
      ByteSwap(&copy, sizeof(TypeToWrite));
   }

   return WriteBytes(&copy, sizeof(TypeToWrite)) == sizeof(TypeToWrite);
}