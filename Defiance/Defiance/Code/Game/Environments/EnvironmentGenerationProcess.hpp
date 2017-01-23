#pragma once

#include <string>


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
struct EnvironmentGenerationProcess
{
   EnvironmentGenerationProcess(const XMLNode& node);
   virtual ~EnvironmentGenerationProcess() {}

   std::string generatorName;
   int numSteps;
};