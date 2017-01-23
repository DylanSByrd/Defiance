#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "ThirdParty/Parsers/XmlParser.h"
#include "Game/Generators/FromDataGenerator.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"
#include "Game/Map/Map.hpp"


//-----------------------------------------------------------------------------------------------
STATIC GeneratorRegistration FromDataGenerator::s_fromDataGeneratorRegistration(
   "FromData", &FromDataGenerator::CreateGenerator, &FromDataGenerator::CreateEnvironmentGenerationProcess);


//-----------------------------------------------------------------------------------------------
FromDataGenerator::FromDataGenerator(const std::string& name)
   : Generator(name)
   // #TODO - move to EGP? I guess...
   , m_mapFilePath("Data/Maps/demo.Map.xml")
{
}


//-----------------------------------------------------------------------------------------------
STATIC EnvironmentGenerationProcess* FromDataGenerator::CreateEnvironmentGenerationProcess(const XMLNode& processNode)
{
   return new FromDataGenerationProcess(processNode);
}


//-----------------------------------------------------------------------------------------------
bool FromDataGenerator::GenerateStep(Map* outMap, int* outCurrentStepNumber, EnvironmentGenerationProcess* process) const
{
   if (*outCurrentStepNumber != 0)
   {
      return false;
   }

   std::string mapName = "default";
   std::string mapFilePath = "Data/Maps/default.Map.xml";

   if (process != nullptr)
   {
      FromDataGenerationProcess* fromDataProcess = dynamic_cast<FromDataGenerationProcess*>(process);
      ASSERT_OR_DIE(fromDataProcess != nullptr, "ERROR: FromDataGenerator did not receive a FromDataGenerationProcess!");

      mapName = fromDataProcess->dataFileNameWithoutExtensions;
      mapFilePath = Stringf("Data/Maps/%s.Map.xml", mapName.c_str());
   }

   ASSERT_OR_DIE(mapFilePath != "", "ERROR: No file path given to FromData generator!");
   XMLNode node = XMLNode::openFileHelper(mapFilePath.c_str());
   XMLNode root = node.getChildNode(0);

   ASSERT_OR_DIE(!root.IsContentEmpty(), "ERROR: Empty xml file given to FromData generator!");
   outMap->InitToXMLNode(root, mapName);

   *outCurrentStepNumber = *outCurrentStepNumber + 1;
   return false;
}


//-----------------------------------------------------------------------------------------------
FromDataGenerationProcess::FromDataGenerationProcess(const XMLNode& node)
   : EnvironmentGenerationProcess(node)
{
   ASSERT_OR_DIE(node.getAttribute("mapName"), "ERROR: No map provided to FromData generator!");
   dataFileNameWithoutExtensions = node.getAttribute("mapName");

   // FromData always runs 1 time
   numSteps = 1;
}