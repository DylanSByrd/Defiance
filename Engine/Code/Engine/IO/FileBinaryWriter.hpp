#pragma once

#include <stdio.h>
#include "Engine/IO/IBinaryWriter.hpp"


//-----------------------------------------------------------------------------------------------
class FileBinaryWriter
   : public IBinaryWriter
{
public:
   FileBinaryWriter() : IBinaryWriter(), m_fileHandle(nullptr) {}
   ~FileBinaryWriter();

   bool OpenFile(const std::string& fileName, bool append = false);
   void CloseFile();

   virtual size_t WriteBytes(const void* source, const size_t numBytesToWrite) override;

private:
   FILE* m_fileHandle;
};