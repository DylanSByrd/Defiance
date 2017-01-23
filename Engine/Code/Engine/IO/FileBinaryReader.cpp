#include "Engine/IO/FileBinaryReader.hpp"


//-----------------------------------------------------------------------------------------------
FileBinaryReader::~FileBinaryReader()
{
   if (m_fileHandle != nullptr)
   {
      CloseFile();
   }
}


//-----------------------------------------------------------------------------------------------
bool FileBinaryReader::OpenFile(const std::string& fileName)
{
   bool fileDidOpen = true;

   const char* readingMode = "rb";

   errno_t error = fopen_s(&m_fileHandle, fileName.c_str(), readingMode);
   {
      if (error != 0)
      {
         return !fileDidOpen;
      }
   }

   return fileDidOpen;
}


//-----------------------------------------------------------------------------------------------
void FileBinaryReader::CloseFile()
{
   if (m_fileHandle != nullptr)
   {
      fclose(m_fileHandle);
      m_fileHandle = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
size_t FileBinaryReader::ReadBytes(void* destination, const size_t numBytesToRead)
{
   size_t numBytesRead = fread(destination, sizeof(byte_t), numBytesToRead, m_fileHandle);
   return numBytesRead;
}