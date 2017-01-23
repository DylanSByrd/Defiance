#pragma once

#include "Game/Generators/Generator.hpp"


//-----------------------------------------------------------------------------------------------
struct EnvironmentGenerationProcess;
struct XMLNode;


//-----------------------------------------------------------------------------------------------
class RiverGenerator
   : public Generator
{
public:
   RiverGenerator(const std::string& name);

   static Generator* CreateGenerator(const std::string& name) { return new RiverGenerator(name); };
   static EnvironmentGenerationProcess* CreateEnvironmentGenerationProcess(const XMLNode& processNode);

   static GeneratorRegistration s_riverGeneratorRegistration;

   void InitializeMap(Map* outMap) const;
   bool GenerateStep(Map* outMap, int* outCurrentStepNumber, EnvironmentGenerationProcess* process = nullptr) const;
};