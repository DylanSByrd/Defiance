#pragma once;

#include <vector>
#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
enum FeatureType
{
   INVALID_FEATURE = -1,
   DOOR_FEATURE,
   NUM_FEATURE_TYPES,
};


//-----------------------------------------------------------------------------------------------
enum FeatureState
{
   INVALID_STATE = -1,

   OPEN_STATE = 0,
   ACTIVATED_STATE = 0,
   
   CLOSED_STATE = 1,
   DEACTIVATED_STATE = 1,
};


//#TODO - Currently only supports an "on" and "off" state, which have multiple names
// Would be nice to have features with more than two states
//-----------------------------------------------------------------------------------------------
class Feature
   : public Entity
{
public:
   Feature(const XMLNode& featureBlueprintNode);
   Feature(const Feature& copySource);
   Feature(const Feature& copySource, const XMLNode& featureLoadNode);

   virtual bool IsFeature() const override { return true; }
   bool IsState(FeatureState state) { return m_state == state; }
   const std::string& GetName() const { return m_name; }
   FeatureState GetCurrentState() const { return m_state; }
   FeatureType GetFeatureType() const { return m_featureType; }
   char GetGlyphForCurrentState() const { return m_glyphs[m_state]; }
   
   virtual void AddToMap(Map* map, const TileCoords& position) override;
   void TogggleState();
   bool DoesBlockLineOfSight();
   bool DoesBlockPathing();

   virtual bool WriteToXMLNode(XMLNode& entityNode) const override;

private:
   virtual void LoadFromXMLNode(const XMLNode& saveNode) override;
   void PopulateFromXMLNode(const XMLNode& blueprintNode);
   void SetFeatureTypeFromString(const std::string& typeString);
   void SetStartingStateFromString(const std::string& startingState);
   void SetStateGlyphsFromString(const std::string& glyphString);
   void SetLineOfSightAndPathingInfoFromNode(const XMLNode& blueprintNode);

   FeatureType m_featureType;
   FeatureState m_state;
   std::vector<char> m_glyphs;

   bool m_doesBlockLineOfSightActive;
   bool m_doesBlockLineOfSightInactive;
   bool m_doesBlockPathingActive;
   bool m_doesBlockPathingInactive;
};