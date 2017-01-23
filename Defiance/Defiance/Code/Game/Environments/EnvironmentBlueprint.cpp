#include <sstream>
#include "ThirdParty/Parsers/XmlParser.h"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/IO/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Game/Generators/Generator.hpp"
#include "Game/Environments/EnvironmentBlueprint.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"


//-----------------------------------------------------------------------------------------------
STATIC EnvironmentBlueprintMap* EnvironmentBlueprint::s_environmentBlueprintMap = nullptr;


//-----------------------------------------------------------------------------------------------
EnvironmentBlueprint::EnvironmentBlueprint(const XMLNode& blueprintNode, const std::string& filenameWithoutExtensions)
{
   // Get Environment details
   const char* environmentName = blueprintNode.getAttribute("name");
   if (environmentName == nullptr || !strcmp(environmentName, ""))
   {
      m_name = filenameWithoutExtensions;
   }
   else
   {
      m_name = environmentName;
   }

   const char* sizeString = blueprintNode.getAttribute("size");
   ASSERT_OR_DIE(sizeString != nullptr, Stringf("ERROR: No size given for environment %s!", m_name.c_str()));
   std::stringstream sizeStream(sizeString);
   
   sizeStream >> m_size.x;
   sizeStream.ignore(1);
   sizeStream >> m_size.y;

   ASSERT_OR_DIE(m_size.x != 0 && m_size.y != 0, Stringf("ERROR: Map with a dimension of size 0 for environment %s!", m_name.c_str()));

   // Build generation processes
   int generationIndex = 0;
   XMLNode generationNode = blueprintNode.getChildNode("GenerationProcessData", &generationIndex);
   while (!generationNode.IsContentEmpty())
   {
      PopulateGenerationProcessFromXMLNode(generationNode);
      generationNode = blueprintNode.getChildNode("GenerationProcessData", &generationIndex);
   }
}


//-----------------------------------------------------------------------------------------------
EnvironmentBlueprint::~EnvironmentBlueprint()
{
   for (EnvironmentGenerationProcess* genProc : m_generationProcesses)
   {
      delete genProc;
      genProc = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
void EnvironmentBlueprint::PopulateGenerationProcessFromXMLNode(const XMLNode& generationNode)
{
   const char*  generatorName = generationNode.getAttribute("generator");
   ASSERT_OR_DIE(generatorName != nullptr, Stringf("ERROR: No generator name specified for %s process number %d!", m_name.c_str(), m_generationProcesses.size() + 1));

   EnvironmentGenerationProcess* generationProcess = GeneratorRegistration::CreateEnvironmentGenerationProcessByName(generatorName, generationNode);
   ASSERT_OR_DIE(generationProcess != nullptr, Stringf("ERROR: Failed to create generation process for %s process number %d!", m_name.c_str(), m_generationProcesses.size() + 1));
   m_generationProcesses.push_back(generationProcess);
}


//-----------------------------------------------------------------------------------------------
STATIC void EnvironmentBlueprint::LoadEnvironmentBlueprints()
{
   std::vector<std::string> blueprintFiles = EnumerateFilesInDirectory("Data/Environments", "*.Environment.xml");

   if (blueprintFiles.size() > 0 && s_environmentBlueprintMap == nullptr)
   {
      s_environmentBlueprintMap = new EnvironmentBlueprintMap;
   }

   for (std::string& blueprintFile : blueprintFiles)
   {
      XMLNode blueprintNode = XMLNode::openFileHelper(blueprintFile.c_str(), "EnvironmentBlueprint");

      int nameStart = blueprintFile.find_last_of('/') + 1;
      std::string environmentName = blueprintFile.substr(nameStart, blueprintFile.size() - nameStart);
      environmentName = environmentName.substr(0, environmentName.find('.'));

      ASSERT_OR_DIE(s_environmentBlueprintMap->find(environmentName) == s_environmentBlueprintMap->end(), Stringf("Error: Environment %s already registered!", environmentName.c_str()));
      EnvironmentBlueprint* newEnvironmentBlueprint = new EnvironmentBlueprint(blueprintNode, environmentName);
      s_environmentBlueprintMap->insert(EnvironmentBlueprintPair(environmentName, newEnvironmentBlueprint));
   }
}


//-----------------------------------------------------------------------------------------------
STATIC void EnvironmentBlueprint::CleanUpEnvironmentBlueprints()
{
   EnvironmentBlueprintMap::iterator blueprintIter = s_environmentBlueprintMap->begin();
   while (blueprintIter != s_environmentBlueprintMap->end())
   {
      delete blueprintIter->second;
      ++blueprintIter;
   }

   delete s_environmentBlueprintMap;
}