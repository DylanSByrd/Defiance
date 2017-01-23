#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/IO/FileUtils.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Entities/Features/FeatureFactory.hpp"


//-----------------------------------------------------------------------------------------------
STATIC FeatureFactoryMap* FeatureFactory::s_factoryMaps[] = { nullptr };


//-----------------------------------------------------------------------------------------------
FeatureFactory::FeatureFactory(const XMLNode& featureBlueprintNode)
   : m_blueprint(nullptr)
{
   PopulateFromXMLNode(featureBlueprintNode);
}


//-----------------------------------------------------------------------------------------------
FeatureFactory::~FeatureFactory()
{
   if (m_blueprint != nullptr)
   {
      delete m_blueprint;
      m_blueprint = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
Feature* FeatureFactory::CreateFeature() const
{
   Feature* newFeature = new Feature(*m_blueprint);
   return newFeature;
}


//-----------------------------------------------------------------------------------------------
Feature* FeatureFactory::CreateFromNode(const XMLNode& node) const
{
   Feature* newFeature = new Feature(*m_blueprint, node);
   return newFeature;
}


//-----------------------------------------------------------------------------------------------
STATIC void FeatureFactory::LoadAllFeatureBlueprints()
{
   std::vector<std::string> featureFiles = EnumerateFilesInDirectory("Data/Features", "*.Feature.xml");

   if (featureFiles.size() > 0 && s_factoryMaps[0] == nullptr)
   {
      for (size_t factoryIndex = 0; factoryIndex < NUM_FEATURE_TYPES; ++factoryIndex)
      {
         s_factoryMaps[factoryIndex] = new FeatureFactoryMap;
      }
   }

   for (const std::string& featureFile : featureFiles)
   {
      XMLNode featureFileNode = XMLNode::openFileHelper(featureFile.c_str(), "FeatureBlueprints");

      int featureIndex = 0;
      XMLNode featureNode = featureFileNode.getChildNode("FeatureBlueprint", &featureIndex);
      while (!featureNode.isEmpty())
      {
         FeatureFactory* newFactory = new FeatureFactory(featureNode);

         std::string featureName = newFactory->m_blueprint->GetName();
         FeatureType newFeatureType = newFactory->m_blueprint->GetFeatureType();

         s_factoryMaps[newFeatureType]->insert(FeatureFactoryPair(featureName, newFactory));
         
         featureNode = featureFileNode.getChildNode("FeatureBlueprint", &featureIndex);
      }
   }
}


//-----------------------------------------------------------------------------------------------
STATIC void FeatureFactory::FreeAllFeatureBlueprints()
{
   for (size_t mapIndex = 0; mapIndex < NUM_FEATURE_TYPES; ++mapIndex)
   {
      FeatureFactoryMap* currentMap = s_factoryMaps[mapIndex];
      FeatureFactoryIter mapIter = currentMap->begin();
      while (mapIter != currentMap->end())
      {
         delete mapIter->second;
         mapIter->second = nullptr;
         ++mapIter;
      }

      delete currentMap;
      s_factoryMaps[mapIndex] = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
STATIC Feature* FeatureFactory::CreateFeatureFromXMLNode(const XMLNode& node)
{
   std::string featureName = node.getAttribute("name");

   for (int mapIndex = 0; mapIndex < NUM_FEATURE_TYPES; ++mapIndex)
   {
      FeatureFactoryMap* factoryMap = s_factoryMaps[mapIndex];
      FeatureFactoryIter factoryIter = factoryMap->find(featureName);

      if (factoryIter != factoryMap->end())
      {
         FeatureFactory* targetFactory = factoryIter->second;
         return targetFactory->CreateFromNode(node);
      }
   }

   return nullptr;
}


//-----------------------------------------------------------------------------------------------
Feature* FeatureFactory::CreateByName(const std::string& name)
{
   for (int mapIndex = 0; mapIndex < NUM_FEATURE_TYPES; ++mapIndex)
   {
      FeatureFactoryMap* factoryMap = s_factoryMaps[mapIndex];
      FeatureFactoryIter factoryIter = factoryMap->find(name);

      if (factoryIter != factoryMap->end())
      {
         FeatureFactory* targetFactory = factoryIter->second;
         return targetFactory->CreateFeature();
      }
   }

   return nullptr;
}


//-----------------------------------------------------------------------------------------------
STATIC Feature* FeatureFactory::CreateByNameOfType(const std::string& name, FeatureType type)
{
   FeatureFactoryMap* factoryMap = s_factoryMaps[type];

   FeatureFactoryIter factoryIter = factoryMap->find(name);
   ASSERT_OR_DIE(factoryIter != factoryMap->end(), Stringf("ERROR: No factory found for Feature %s", name.c_str()));

   FeatureFactory* targetFactory = factoryIter->second;
   return targetFactory->CreateFeature();
}


//-----------------------------------------------------------------------------------------------
STATIC Feature* FeatureFactory::CreateRandomFeature()
{
   IntRange range(0, NUM_FEATURE_TYPES - 1);
   FeatureType type = (FeatureType)GetRandomIntBetweenInclusive(range);
   return CreateRandomFeatureOfType(type);
}


//-----------------------------------------------------------------------------------------------
STATIC Feature* FeatureFactory::CreateRandomFeatureOfType(FeatureType type)
{
   FeatureFactoryMap* factoryMap = s_factoryMaps[type];

   IntRange range(0, factoryMap->size() - 1);
   FeatureFactoryIter factoryIter = factoryMap->begin();
   std::advance(factoryIter, GetRandomIntBetweenInclusive(range));

   return factoryIter->second->CreateFeature();
}


//-----------------------------------------------------------------------------------------------
void FeatureFactory::PopulateFromXMLNode(const XMLNode& featureBlueprintNode)
{
   m_blueprint = new Feature(featureBlueprintNode);
}


