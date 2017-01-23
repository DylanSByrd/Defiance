#include "Engine/IO/IBinaryWriter.hpp"


//-----------------------------------------------------------------------------------------------
bool IBinaryWriter::WriteString(const std::string& stringToWrite)
{
   size_t length = stringToWrite.length();
   size_t bufferLength = length + 1;
   bool isWriteSuccess =  Write<uint32_t>(bufferLength) && (WriteBytes(stringToWrite.c_str(), bufferLength) == bufferLength);
   return isWriteSuccess;
}


//-----------------------------------------------------------------------------------------------
bool IBinaryWriter::WriteInt(int intToWrite)
{
   bool isWriteSuccess = Write(intToWrite);
   return isWriteSuccess;
}


//-----------------------------------------------------------------------------------------------
bool IBinaryWriter::WriteFloat(float floatToWrite)
{
   bool isWriteSuccess = Write(floatToWrite);
   return isWriteSuccess;
}