#include <sstream>
#include "Engine/Debug/ConsoleCommand.hpp"


//-----------------------------------------------------------------------------------------------
void ConsoleCommand::InitCommandToString(const std::string& commandString)
{
   int nameLength = commandString.find_first_of(" ");
   m_name = commandString.substr(0, nameLength);

   if (nameLength == -1)
   {
      return;
   }

   std::string argsAsString = commandString.substr(nameLength + 1, commandString.length() - nameLength);
   m_args.SetArgs(argsAsString);
}


//-----------------------------------------------------------------------------------------------
int ConsoleCommand::GetArgCount() const
{
   return m_args.GetArgCount();
}