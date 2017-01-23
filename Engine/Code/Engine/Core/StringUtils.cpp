#include <stdarg.h>
#include <ctype.h>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Tokenizer.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Math/Vector2i.hpp"
#include "Engine/Math/IntRange.hpp"


// Provided by #Eiserloh
//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
   char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
   va_list variableArgumentList;
   va_start( variableArgumentList, format );
   vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
   va_end( variableArgumentList );
   textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

   return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
   char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
   char* textLiteral = textLiteralSmall;
   if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
      textLiteral = new char[ maxLength ];

   va_list variableArgumentList;
   va_start( variableArgumentList, format );
   vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
   va_end( variableArgumentList );
   textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

   std::string returnValue( textLiteral );
   if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
      delete[] textLiteral;

   return returnValue;
}


//-----------------------------------------------------------------------------------------------
const std::string ToLowerCase(const std::string& inString)
{
   std::string outString = "";
   for (unsigned stringIndex = 0; stringIndex < inString.length(); ++stringIndex)
   {
      outString += (char)tolower(inString[stringIndex]);
   }
   return outString;
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString(int& outValue, const std::string& valueAsString)
{
   outValue = atoi(valueAsString.c_str());
}


void SetTypeFromString(float& outValue, const std::string& valueAsString)
{
   outValue = (float)atof(valueAsString.c_str());
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString(char& outValue, const std::string& valueAsString)
{
   outValue = valueAsString[0];
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString(unsigned char& outValue, const std::string& valueAsString)
{
   outValue = (unsigned char)atoi(valueAsString.c_str());
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString(Rgba& outValue, const std::string& valueAsString)
{
   if (!strcmp(valueAsString.c_str(), ""))
   {
      return;
   }

   size_t firstCommaOffset = valueAsString.find(',');
   size_t secondCommaOffset = valueAsString.find(',', firstCommaOffset + 1);
   size_t thirdCommaOffset = valueAsString.find('r', secondCommaOffset + 1);

   ASSERT_OR_DIE(firstCommaOffset != std::string::npos && secondCommaOffset != std::string::npos, Stringf("ERROR: Invald format to get color from %s", valueAsString.c_str()));

   std::string redAsString = valueAsString.substr(0, firstCommaOffset);
   SetTypeFromString(outValue.r, redAsString);

   std::string greenAsString = valueAsString.substr(firstCommaOffset + 1, secondCommaOffset - (firstCommaOffset + 1));
   SetTypeFromString(outValue.g, greenAsString);
 
   if (thirdCommaOffset == std::string::npos)
   {
      std::string blueAsString = valueAsString.substr(secondCommaOffset + 1, valueAsString.length() - (secondCommaOffset + 1));
      SetTypeFromString(outValue.b, blueAsString);
      outValue.a = 255;
   }
   else
   {
      std::string blueAsString = valueAsString.substr(secondCommaOffset + 1, thirdCommaOffset - (secondCommaOffset + 1));
      SetTypeFromString(outValue.b, blueAsString);

      std::string alphaAsString = valueAsString.substr(thirdCommaOffset + 1, valueAsString.length() - (thirdCommaOffset + 1));
      SetTypeFromString(outValue.a, alphaAsString);
   }
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString(std::string& outValue, const std::string& valueAsString)
{
   outValue = valueAsString;
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString(Vector2i& outValue, const std::string& valueAsString)
{
   Tokenizer vecToken(valueAsString, ",");

   SetTypeFromString(outValue.x, vecToken.GetNextToken());
   SetTypeFromString(outValue.y, vecToken.GetNextToken());
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString(IntRange& outValue, const std::string& valueAsString)
{
   Tokenizer vecToken(valueAsString, "~");

   SetTypeFromString(outValue.x, vecToken.GetNextToken());
   SetTypeFromString(outValue.y, vecToken.GetNextToken());
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString(bool& outValue, const std::string& valueAsString)
{
   if (!strcmp(valueAsString.c_str(), ""))
   {
      return;
   }

   if (!strcmp(valueAsString.c_str(), "true"))
   {
      outValue = true;
      return;
   }
   else if (!strcmp(valueAsString.c_str(), "false"))
   {
      outValue = false;
      return;
   }

   ERROR_AND_DIE(Stringf("Invalid boolean parse attempt for string %s!", valueAsString.c_str()));
}

