#pragma once

#include <string>


//-----------------------------------------------------------------------------------------------
class Rgba;
class Vector2i;
class IntRange;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );


//-----------------------------------------------------------------------------------------------
const std::string ToLowerCase(const std::string& inString);


//-----------------------------------------------------------------------------------------------
void SetTypeFromString(int& outValue, const std::string& valueAsString);
void SetTypeFromString(float& outValue, const std::string& valueAsString);
void SetTypeFromString(char& outValue, const std::string& valueAsString);
void SetTypeFromString(unsigned char& outValue, const std::string& valueAsString);
void SetTypeFromString(Rgba& outValue, const std::string& valueAsString);
void SetTypeFromString(std::string& outValue, const std::string& valueAsString);
void SetTypeFromString(Vector2i& outValue, const std::string& valueAsString);
void SetTypeFromString(IntRange& outValue, const std::string& valueAsString);
void SetTypeFromString(bool& outValue, const std::string& valueAsString);