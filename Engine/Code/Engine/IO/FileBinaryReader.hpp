#pragma once

#include <stdio.h>
#include "Engine/IO/IBinaryReader.hpp"


//-----------------------------------------------------------------------------------------------
class FileBinaryReader
   : public IBinaryReader
{
public:
   FileBinaryReader() : IBinaryReader(), m_fileHandle(nullptr) {}
   ~FileBinaryReader();

   bool OpenFile(const std::string& fileName);
   void CloseFile();

   virtual size_t ReadBytes(void* destination, const size_t numberOfBytesToRead) override;

private:
   FILE* m_fileHandle;
};