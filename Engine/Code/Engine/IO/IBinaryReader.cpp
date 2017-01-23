#include "Engine/IO/IBinaryReader.hpp"


// #DEBUG - probably doesn't accurately read in a single null char
//-----------------------------------------------------------------------------------------------
bool IBinaryReader::ReadString(std::string* outString)
{
   bool isReadSuccess = true;

   size_t stringLength;
   Read<uint32_t>(&stringLength);
   
   if (stringLength == 0U)
   {
      return isReadSuccess;
   }

   outString->resize(stringLength);
   outString->shrink_to_fit();
   bool isStringReadSucess = (ReadBytes(&((*outString)[0]), stringLength) == stringLength);

   return isStringReadSucess;
}


//-----------------------------------------------------------------------------------------------
bool IBinaryReader::ReadInt(int* outInt)
{
   bool isReadSuccess = Read(outInt);
   return isReadSuccess;
}


//-----------------------------------------------------------------------------------------------
bool IBinaryReader::ReadFloat(float* outFloat)
{
   bool isReadSuccess = Read(outFloat);
   return isReadSuccess;
}