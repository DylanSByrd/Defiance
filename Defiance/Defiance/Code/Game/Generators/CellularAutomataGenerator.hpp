#pragma once

#include "Game/Generators/Generator.hpp"


//-----------------------------------------------------------------------------------------------
struct EnvironmentGenerationProcess;
struct XMLNode;


//-----------------------------------------------------------------------------------------------
class CellularAutomataGenerator
   : public Generator
{
public:
   CellularAutomataGenerator(const std::string& name);

   static Generator* CreateGenerator(const std::string& name) { return new CellularAutomataGenerator(name); };
   static EnvironmentGenerationProcess* CreateEnvironmentGenerationProcess(const XMLNode& processNode);

   static GeneratorRegistration s_cellularAutomataGeneratorRegistration;

   void InitializeMap(Map* outMap) const;
   bool GenerateStep(Map* outMap, int* outCurrentStepNumber, EnvironmentGenerationProcess* process = nullptr) const;
};