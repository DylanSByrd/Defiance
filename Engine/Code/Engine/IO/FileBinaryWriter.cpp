#include "Engine/IO/FileBinaryWriter.hpp"


//-----------------------------------------------------------------------------------------------
FileBinaryWriter::~FileBinaryWriter()
{
   if (m_fileHandle != nullptr)
   {
      CloseFile();
   }
}


//-----------------------------------------------------------------------------------------------
bool FileBinaryWriter::OpenFile(const std::string& fileName, bool append)
{
   bool fileDidOpen = true;

   const char* writingMode;
   if (append)
   {
      writingMode = "ab";
   }
   else
   {
      writingMode = "wb";
   }


   errno_t error = fopen_s(&m_fileHandle, fileName.c_str(), writingMode);
   {
      if (error != 0)
      {
         return !fileDidOpen;
      }
   }

   return fileDidOpen;
}


//-----------------------------------------------------------------------------------------------
void FileBinaryWriter::CloseFile()
{
   if (m_fileHandle != nullptr)
   {
      fclose(m_fileHandle);
      m_fileHandle = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
size_t FileBinaryWriter::WriteBytes(const void* source, const size_t numBytesToWrite)
{
   size_t numBytesWritten = fwrite(source, sizeof(byte_t), numBytesToWrite, m_fileHandle);
   return numBytesWritten;
}