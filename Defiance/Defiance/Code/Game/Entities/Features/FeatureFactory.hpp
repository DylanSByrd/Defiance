#pragma once

#include <map>
#include <string.h>
#include "Game/Entities/Features/Feature.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class FeatureFactory;
typedef std::map<std::string, FeatureFactory*> FeatureFactoryMap;


//-----------------------------------------------------------------------------------------------
class FeatureFactory
{
public:
   FeatureFactory(const XMLNode& featureBlueprintNode);
   ~FeatureFactory();

   Feature* CreateFeature() const;
   Feature* CreateFromNode(const XMLNode& node) const;

   static void LoadAllFeatureBlueprints();
   static void FreeAllFeatureBlueprints();
   static Feature* CreateFeatureFromXMLNode(const XMLNode& node);
   static Feature* CreateByName(const std::string& name);
   static Feature* CreateByNameOfType(const std::string& name, FeatureType type);
   static Feature* CreateRandomFeature();
   static Feature* CreateRandomFeatureOfType(FeatureType type);
   static FeatureFactoryMap* s_factoryMaps[NUM_FEATURE_TYPES];

private:
   void PopulateFromXMLNode(const XMLNode& featureBlueprintNode);

   Feature* m_blueprint;
};
typedef std::pair<std::string, FeatureFactory*> FeatureFactoryPair;
typedef FeatureFactoryMap::iterator FeatureFactoryIter;