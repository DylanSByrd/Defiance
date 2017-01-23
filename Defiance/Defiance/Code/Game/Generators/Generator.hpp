#pragma once

#include <string>
#include <map>


//-----------------------------------------------------------------------------------------------
class Map;
class Vector2i;
struct XMLNode;
struct EnvironmentGenerationProcess;


//-----------------------------------------------------------------------------------------------
class Generator
{
public:
   Generator(const std::string& name);
   virtual ~Generator() {}

   Map* GenerateEmptyMap(const Vector2i& dimensions, const std::string& name) const;
   static void FinalizeMap(Map* outMap);

   virtual void InitializeMap(Map* outMap) const = 0;
   virtual bool GenerateStep(Map* outMap, int* outCurrentStepNumber, EnvironmentGenerationProcess* process = nullptr) const = 0;

   const std::string& GetName() const { return m_name; }

private:
   std::string m_name;
};


//-----------------------------------------------------------------------------------------------
typedef Generator* (GeneratorCreationFunc)(const std::string& name);
typedef EnvironmentGenerationProcess* (GenerationProcessCreationFunc)(const XMLNode& node);


//-----------------------------------------------------------------------------------------------
class GeneratorRegistration;
typedef std::map<std::string, GeneratorRegistration*> GeneratorRegistrationMap;


//-----------------------------------------------------------------------------------------------
class GeneratorRegistration
{
public:
   GeneratorRegistration(const std::string& name, GeneratorCreationFunc* creationFunc, GenerationProcessCreationFunc* processCreationFunc);
   GeneratorRegistration() {}
   static Generator* CreateGeneratorByName(const std::string& name);
   static EnvironmentGenerationProcess* CreateEnvironmentGenerationProcessByName(const std::string& name, const XMLNode& node);
   static void FreeAllGeneratorRegistrations();
   static GeneratorRegistrationMap* s_generatorRegistrationMap;

private:
   std::string m_name;
   GeneratorCreationFunc* m_creationFunc;
   GenerationProcessCreationFunc* m_processCreationFunc;
};
typedef std::pair<std::string, GeneratorRegistration*> GeneratorRegistrationPair;