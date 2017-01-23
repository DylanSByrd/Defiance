#pragma once

#include <map>
#include <string>
#include <vector>
#include "Engine/Math/Vector2i.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
class EnvironmentBlueprint;
typedef std::map<std::string, EnvironmentBlueprint*> EnvironmentBlueprintMap;
typedef std::pair<std::string, EnvironmentBlueprint*> EnvironmentBlueprintPair;

//-----------------------------------------------------------------------------------------------
class EnvironmentBlueprint
{
public:
   EnvironmentBlueprint(const XMLNode& blueprintNode, const std::string& filenameWithoutExtensions);
   ~EnvironmentBlueprint();

   void PopulateGenerationProcessFromXMLNode(const XMLNode& generationNode);
   const std::vector<EnvironmentGenerationProcess*>& GetGenerationProcesses() const { return m_generationProcesses; }
   const std::string& GetName() const { return m_name; }
   const Vector2i& GetSize() const { return m_size; }

   static void LoadEnvironmentBlueprints(); // optional - add a path in case an environment is in a different folder
   static void CleanUpEnvironmentBlueprints();
   static EnvironmentBlueprintMap* s_environmentBlueprintMap;

protected:
   std::string m_name;
   Vector2i m_size;
   std::vector<EnvironmentGenerationProcess*> m_generationProcesses;
};