#pragma once

#include <string>
#include <vector>
#include "Engine/Debug/ConsoleCommandArgs.hpp"


//-----------------------------------------------------------------------------------------------
class ConsoleCommand
{
public:
   ConsoleCommand() : m_name(), m_args() {}
   ~ConsoleCommand() {}

   void InitCommandToString(const std::string& commandString);
   const std::string& GetCommandName() const { return m_name; }
   ConsoleCommandArgs& GetCommandArgs() { return m_args; }
   int GetArgCount() const;

private:
   std::string m_name;
   ConsoleCommandArgs m_args;
};