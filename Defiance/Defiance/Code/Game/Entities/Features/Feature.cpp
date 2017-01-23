#include "Engine/Parsers/XMLUtilities.hpp"
#include "Engine/Core/Tokenizer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Entities/Features/Feature.hpp"
#include "Game/Map/Map.hpp"


//-----------------------------------------------------------------------------------------------
Feature::Feature(const XMLNode& blueprintNode)
   : Entity(blueprintNode)
   , m_featureType(INVALID_FEATURE)
   , m_state(INVALID_STATE)
   , m_doesBlockLineOfSightActive(false)
   , m_doesBlockLineOfSightInactive(false)
   , m_doesBlockPathingActive(false)
   , m_doesBlockPathingInactive(false)
{
   PopulateFromXMLNode(blueprintNode);
}


//-----------------------------------------------------------------------------------------------
Feature::Feature(const Feature& copySource)
   : Entity(copySource)
   , m_featureType(copySource.m_featureType)
   , m_state(copySource.m_state)
   , m_glyphs(copySource.m_glyphs)
   , m_doesBlockLineOfSightActive(copySource.m_doesBlockLineOfSightActive)
   , m_doesBlockLineOfSightInactive(copySource.m_doesBlockLineOfSightInactive)
   , m_doesBlockPathingActive(copySource.m_doesBlockPathingActive)
   , m_doesBlockPathingInactive(copySource.m_doesBlockPathingInactive)
{}



//-----------------------------------------------------------------------------------------------
Feature::Feature(const Feature& copySource, const XMLNode& featureLoadNode)
   : Feature(copySource)
{
   LoadFromXMLNode(featureLoadNode);
}


//-----------------------------------------------------------------------------------------------
void Feature::AddToMap(Map* map, const TileCoords& position)
{
   Entity::AddToMap(map, position);
   map->AddFeature(this, position);
}


//-----------------------------------------------------------------------------------------------
void Feature::TogggleState()
{
   switch (m_state)
   {
   // case ACTIVATED_STATE:
   case OPEN_STATE:
   {
      // m_state = DEACTIVATED_STATE;
      m_state = CLOSED_STATE;
      return;
   }

   // case DEACTIVATED_STATE:
   case CLOSED_STATE:
   {
      // m_state = ACTIVATED_STATE;
      m_state = OPEN_STATE;
      return;
   }
   }
}


//-----------------------------------------------------------------------------------------------
bool Feature::DoesBlockLineOfSight()
{
   if (m_state == ACTIVATED_STATE)
   {
      return m_doesBlockLineOfSightActive;
   }

   return m_doesBlockLineOfSightInactive;
}


//-----------------------------------------------------------------------------------------------
bool Feature::DoesBlockPathing()
{
   if (m_state == ACTIVATED_STATE)
   {
      return m_doesBlockPathingActive;
   }

   return m_doesBlockPathingInactive;
}


//-----------------------------------------------------------------------------------------------
bool Feature::WriteToXMLNode(XMLNode& entityNode) const
{
   bool writeSuccessful = true;

   XMLNode featureNode = entityNode.addChild("FeatureBlueprint");
   if (!Entity::WriteToXMLNode(featureNode))
   {
      return !writeSuccessful;
   }

   switch (m_state)
   {
   case OPEN_STATE:
   {
      featureNode.addAttribute("state", "open");
      break;
   }

   case CLOSED_STATE:
   {
      featureNode.addAttribute("state", "closed");
      break;
   }
   }

   return writeSuccessful;
}


//-----------------------------------------------------------------------------------------------
void Feature::LoadFromXMLNode(const XMLNode& saveNode)
{
   Entity::LoadFromXMLNode(saveNode);

   std::string stateAsString = ReadXMLAttribute(saveNode, "state", std::string("closed"));
   SetStartingStateFromString(stateAsString);
}


//-----------------------------------------------------------------------------------------------
void Feature::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
   SetFeatureTypeFromString(ReadXMLAttribute(blueprintNode, "type", std::string("invalid")));
   SetStartingStateFromString(ReadXMLAttribute(blueprintNode, "startingState", std::string("invalid")));
   SetStateGlyphsFromString(ReadXMLAttribute(blueprintNode, "stateGlyphs", std::string("?,?")));
   SetLineOfSightAndPathingInfoFromNode(blueprintNode);
}


//-----------------------------------------------------------------------------------------------
void Feature::SetFeatureTypeFromString(const std::string& typeString)
{
   if (!strcmp(typeString.c_str(), "door"))
   {
      m_featureType = DOOR_FEATURE;
      return;
   }

   m_featureType = INVALID_FEATURE;
   return;
}


//-----------------------------------------------------------------------------------------------
void Feature::SetStartingStateFromString(const std::string& startingState)
{
   if (!strcmp(startingState.c_str(), "closed"))
   {
      m_state = CLOSED_STATE;
      return;
   }

   if (!strcmp(startingState.c_str(), "open"))
   {
      m_state = OPEN_STATE;
      return;
   }

   if (!strcmp(startingState.c_str(), "deactivated"))
   {
      m_state = DEACTIVATED_STATE;
      return;
   }

   if (!strcmp(startingState.c_str(), "activated"))
   {
      m_state = ACTIVATED_STATE;
      return;
   }

   m_state = INVALID_STATE;
   return;
}


//-----------------------------------------------------------------------------------------------
void Feature::SetStateGlyphsFromString(const std::string& glyphString)
{
   Tokenizer glyphTokens(glyphString, ",");
   while (!glyphTokens.IsFinished())
   {
      std::string tokenAsString = glyphTokens.GetNextToken();
      m_glyphs.push_back(tokenAsString[0]);
   }
}


//-----------------------------------------------------------------------------------------------
void Feature::SetLineOfSightAndPathingInfoFromNode(const XMLNode& blueprintNode)
{
   std::string lineOfSightString = ReadXMLAttribute(blueprintNode, "doesBlockLineOfSight", std::string("false, false"));
   Tokenizer lineOfSightTokens(lineOfSightString, ",");

   std::string lineOfSightActiveString = lineOfSightTokens.GetNextToken();
   SetTypeFromString(m_doesBlockLineOfSightActive, lineOfSightActiveString);
   std::string lineOfSightInactiveString = lineOfSightTokens.GetNextToken();
   SetTypeFromString(m_doesBlockLineOfSightInactive, lineOfSightInactiveString);
   
   std::string pathingString = ReadXMLAttribute(blueprintNode, "doesBlockPathing", std::string("false, false"));
   Tokenizer pathingTokens(pathingString, ",");

   std::string pathingActiveString = pathingTokens.GetNextToken();
   SetTypeFromString(m_doesBlockPathingActive, pathingActiveString);
   std::string pathingInactiveString = pathingTokens.GetNextToken();
   SetTypeFromString(m_doesBlockPathingInactive, pathingInactiveString);
}