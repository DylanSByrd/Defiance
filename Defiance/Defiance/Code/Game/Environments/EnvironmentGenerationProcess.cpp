#include <sstream>
#include "ThirdParty/Parsers/XmlParser.h"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"


//-----------------------------------------------------------------------------------------------
EnvironmentGenerationProcess::EnvironmentGenerationProcess(const XMLNode& node)
   : generatorName("")
   , numSteps(0)
{
   // This is checked earlier in EnvironmentBlueprint, but for my sanity:
   ASSERT_OR_DIE(node.getAttribute("generator"), "ERROR: No generator associated with generation process!");
   generatorName = node.getAttribute("generator");

   // if no steps provided; use the default values given in the generator
   if (node.getAttribute("steps"))
   {
      std::string stepsString = node.getAttribute("steps");
      if (stepsString.find('~') == -1)
      {
         int steps = std::stoi(stepsString);
         numSteps = steps;
      }
      else // presence of '~' means a range
      {
         int minSteps = 0;
         int maxSteps = 0;
         std::stringstream stepsStream(stepsString);
         stepsStream >> minSteps;
         stepsStream.ignore(1);
         stepsStream >> maxSteps;

         numSteps = GetRandomIntBetweenInclusive(minSteps, maxSteps);
      }
   }
}