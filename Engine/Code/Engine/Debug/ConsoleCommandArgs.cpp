#include <sstream>
#include "Engine/Debug/ConsoleCommandArgs.hpp"


//-----------------------------------------------------------------------------------------------
void ConsoleCommandArgs::SetArgs(const std::string& argString)
{
   m_argsString = argString;

   std::string currentArg = "";
   std::stringstream argStream(argString);

   while (argStream >> currentArg)
   {
      m_argsList.push_back(currentArg);
   }
}


//-----------------------------------------------------------------------------------------------
int ConsoleCommandArgs::GetArgCount() const
{
   return m_argsList.size();
}


//-----------------------------------------------------------------------------------------------
bool ConsoleCommandArgs::GetNextArgAsString(std::string* stringOut, const std::string& defaultValue)
{
   bool isSuccess = true;

   if (m_argPosition >= GetArgCount())
   {
      *stringOut = defaultValue;
      return !isSuccess;
   }

   std::stringstream argStream(m_argsList[m_argPosition]);
   ++m_argPosition;

   if (argStream >> *stringOut)
   {
      return isSuccess;
   }
   else
   {
      *stringOut = defaultValue;
      return !isSuccess;
   }
}


//-----------------------------------------------------------------------------------------------
bool ConsoleCommandArgs::GetNextArgAsInt(int* intOut, int defaultValue)
{
   bool isSuccess = true;

   if (m_argPosition >= GetArgCount())
   {
      *intOut = defaultValue;
      return !isSuccess;
   }

   std::stringstream argStream(m_argsList[m_argPosition]);
   ++m_argPosition;

   if (argStream >> *intOut)
   {
      return isSuccess;
   }
   else
   {
      *intOut = defaultValue;
      return !isSuccess;
   }
}


//-----------------------------------------------------------------------------------------------
bool ConsoleCommandArgs::GetNextArgAsFloat(float* floatOut, float defaultValue)
{
   bool isSuccess = true;

   if (m_argPosition >= GetArgCount())
   {
      *floatOut = defaultValue;
      return !isSuccess;
   }

   std::stringstream argStream(m_argsList[m_argPosition]);
   ++m_argPosition;
   
   if (argStream >> *floatOut)
   {
      return isSuccess;
   }
   else
   {
      *floatOut = defaultValue;
      return !isSuccess;
   }
}