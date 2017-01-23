#pragma once

#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class ConsoleCommandArgs
{
public:
   ConsoleCommandArgs() : m_argsString(), m_argsList(), m_argPosition(0) {}
   ~ConsoleCommandArgs() {}

   void SetArgs(const std::string& argString);
   int GetArgCount() const;
   bool GetNextArgAsString(std::string* stringOut, const std::string& defaultValue);
   bool GetNextArgAsInt(int* intOut, int defaultValue);
   bool GetNextArgAsFloat(float* floatOut, float defaultValue);

private:
   std::string m_argsString;
   std::vector<std::string> m_argsList;
   int m_argPosition;
};
typedef void (ConsoleCommandFunction)(ConsoleCommandArgs& args);
