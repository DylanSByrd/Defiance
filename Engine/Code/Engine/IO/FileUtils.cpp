#include <io.h>
#include "Engine/IO/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
bool LoadBinaryFileToBuffer(const std::string& filePath, std::vector<unsigned char>& out_buffer)
{
   FILE* fileToLoad = nullptr;

   if (fopen_s(&fileToLoad, filePath.c_str(), "rb"))
   {
      return false;
   }

   fseek(fileToLoad, SEEK_SET, SEEK_END);
   int size = ftell(fileToLoad);
   rewind(fileToLoad);

   out_buffer.resize(size);
   fread(&out_buffer[0], 1, size, fileToLoad);
   fclose(fileToLoad);
   return true;
}


//-----------------------------------------------------------------------------------------------
bool LoadBinaryFileToBuffer(const std::string& filePath, std::vector<float>& out_buffer)
{
   FILE* fileToLoad = nullptr;

   if (fopen_s(&fileToLoad, filePath.c_str(), "rb"))
   {
      return false;
   }

   fseek(fileToLoad, SEEK_SET, SEEK_END);
   int size = ftell(fileToLoad);
   rewind(fileToLoad);

   out_buffer.resize(size);
   fread(&out_buffer[0], 4, size, fileToLoad);
   fclose(fileToLoad);
   return true;
}


//-----------------------------------------------------------------------------------------------
bool LoadTextFileToBuffer(const std::string& filePath, std::string& out_buffer)
{
   FILE* fileToLoad = nullptr;
   errno_t err = fopen_s(&fileToLoad, filePath.c_str(), "rb");
   if (err)
   {
      return false;
   }

   fseek(fileToLoad, SEEK_SET, SEEK_END);
   int size = ftell(fileToLoad);
   rewind(fileToLoad);

   out_buffer.resize(size);
   fread(&out_buffer[0], 4, size, fileToLoad);
   fclose(fileToLoad);
   return true;
}


//-----------------------------------------------------------------------------------------------
bool SaveBufferToBinaryFile(const std::string& filePath, const std::vector<unsigned char>& buffer)
{
   FILE* file = nullptr;

   if (fopen_s(&file, filePath.c_str(), "wb"))
   {
      return false;
   }

   fwrite(&buffer[0], 1, buffer.size(), file);
   fclose(file);
   return true;
}


//-----------------------------------------------------------------------------------------------
bool SaveBufferToBinaryFile(const std::string& filePath, const std::vector<float>& buffer)
{
   FILE* file = nullptr;

   if (fopen_s(&file, filePath.c_str(), "wb"))
   {
      return false;
   }

   fwrite(&buffer[0], 4, buffer.size(), file);
   fclose(file);
   return true;
}

//-----------------------------------------------------------------------------------------------
const char* GetFullPath(const std::string& relativePath)
{
   return _fullpath(NULL, relativePath.c_str(), _MAX_PATH);
}


//-----------------------------------------------------------------------------------------------
std::vector<std::string> EnumerateFilesInDirectory(const std::string& relativeDirectoryPath, const std::string& filePattern)
{
   std::string searchPathPattern = relativeDirectoryPath + "/" + filePattern;
   std::vector<std::string> foundFiles;

   int error = 0;
   struct _finddata_t fileInfo;
   intptr_t searchHandle = _findfirst(searchPathPattern.c_str(), &fileInfo);
   while (searchHandle != -1 && !error)
   {
      std::string relativePathToFile = Stringf("%s/%s", relativeDirectoryPath.c_str(), fileInfo.name);
      bool isDirectory = fileInfo.attrib & _A_SUBDIR ? true : false;
      bool isHidden = fileInfo.attrib & _A_HIDDEN ? true : false;

      if (!isDirectory && !isHidden)
      {
         foundFiles.push_back(relativePathToFile);

      }
      error = _findnext(searchHandle, &fileInfo);
   }
      _findclose(searchHandle);

      return foundFiles;
}

