#pragma once

#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
bool LoadBinaryFileToBuffer(const std::string& filePath, std::vector<unsigned char>& out_buffer);
bool LoadBinaryFileToBuffer(const std::string& filePath, std::vector<float>& out_buffer);
bool LoadTextFileToBuffer(const std::string& filePath, std::string& out_buffer);


//-----------------------------------------------------------------------------------------------
bool SaveBufferToBinaryFile(const std::string& filePath, const std::vector<unsigned char>& buffer);
bool SaveBufferToBinaryFile(const std::string& filePath, const std::vector<float>& buffer);


//-----------------------------------------------------------------------------------------------
const char* GetFullPath(const std::string& relativePath);
std::vector<std::string> EnumerateFilesInDirectory(const std::string& relativeDirectoryPath, const std::string& filePattern);