#pragma once

#include "Game/Generators/Generator.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class Vector2i;
typedef Vector2i TileCoords;


//-----------------------------------------------------------------------------------------------
class FromDataGenerator
   : public Generator
{
public:
   FromDataGenerator(const std::string& name);

   static Generator* CreateGenerator(const std::string& name) { return new FromDataGenerator(name); };
   static EnvironmentGenerationProcess* CreateEnvironmentGenerationProcess(const XMLNode& processNode);

   static GeneratorRegistration s_fromDataGeneratorRegistration;

   void SetMapFilePath(const std::string& path) { m_mapFilePath = path; }
   void InitializeMap(Map*) const {} // Special case
   bool GenerateStep(Map* outMap, int* outCurrentStepNumber, EnvironmentGenerationProcess*) const;

private:
   std::string m_mapFilePath;
};


//-----------------------------------------------------------------------------------------------
struct FromDataGenerationProcess
   : public EnvironmentGenerationProcess
{
   FromDataGenerationProcess(const XMLNode& node);

   std::string dataFileNameWithoutExtensions;
};