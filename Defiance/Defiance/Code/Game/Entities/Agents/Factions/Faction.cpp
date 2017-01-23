#include <vector>
#include <string>
#include <sstream>
#include "Engine/IO/FileUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Tokenizer.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Agents/Factions/Faction.hpp"
#include "Game/Entities/Agents/Agent.hpp"
#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
STATIC int Faction::s_nextID = 1;
STATIC const int Faction::INVALID_FACTION_ID = 0;
STATIC FactionMap* Faction::s_globalFactionMap = nullptr;


//-----------------------------------------------------------------------------------------------
Faction::Faction()
   : m_name("Uninitialized Faction")
   , m_factionID(INVALID_FACTION_ID)
{}


//-----------------------------------------------------------------------------------------------
Faction::Faction(const std::string& name)
   : m_name(name)
   , m_factionID(s_nextID++)
{}


//-----------------------------------------------------------------------------------------------
void Faction::PopulateFromXMLNode(const XMLNode& factionNode)
{
   PopulateGivenStatusFromNode(factionNode, FACTION_STATUS_DISLIKES);
   PopulateGivenStatusFromNode(factionNode, FACTION_STATUS_HATES);
   PopulateGivenStatusFromNode(factionNode, FACTION_STATUS_NEUTRAL);
   PopulateGivenStatusFromNode(factionNode, FACTION_STATUS_LIKES);
   PopulateGivenStatusFromNode(factionNode, FACTION_STATUS_LOVES);
}


//-----------------------------------------------------------------------------------------------
void Faction::PopulateGivenStatusFromNode(const XMLNode& factionNode, FactionStatus status)
{
   std::string statusAsString = GetFactionStatusAsString(status);
   std::string listOfStatus = ReadXMLAttribute(factionNode, statusAsString, std::string(""));
   Tokenizer statusTokens(listOfStatus, ", ");
   std::string statusFactionName = statusTokens.GetNextToken();

   while (statusFactionName.compare(""))
   {
      Faction* dislikedFaction = CreateOrGetFactionByName(statusFactionName);
      FactionRelationship newDislikeRelationship = { status, statusFactionName };
      FactionRelationshipPair dislikedFactionPair(dislikedFaction->GetFactionID(), newDislikeRelationship);
      m_factionRelations.insert(dislikedFactionPair);

      statusFactionName = statusTokens.GetNextToken();
   }
}


//-----------------------------------------------------------------------------------------------
bool Faction::IsAgentRelationPresent(EntityID agentID) const
{
   bool isAgentPresent = (m_agentRelations.find(agentID) != m_agentRelations.end());
   return isAgentPresent;
}


//-----------------------------------------------------------------------------------------------
bool Faction::IsFactionRelationPresent(FactionID factionID) const
{
   bool isFactionPresent = (m_factionRelations.find(factionID) != m_factionRelations.end());
   return isFactionPresent;
}


//-----------------------------------------------------------------------------------------------
void Faction::AddAgentRelation(EntityID agentID, const std::string& name, int standing)
{
   FactionRelationship newRelationship = { standing, name };
   FactionRelationshipPair newAgent(agentID, newRelationship);
   m_agentRelations.insert(newAgent);
}


//-----------------------------------------------------------------------------------------------
void Faction::AddFactionRelation(FactionID factionID, const std::string& name, int standing)
{
   FactionRelationship newRelationship = { standing, name };
   FactionRelationshipPair newFaction(factionID, newRelationship);
   m_factionRelations.insert(newFaction);
}


//-----------------------------------------------------------------------------------------------
int Faction::GetFactionStandingWithAgent(EntityID agentID) const
{
   return m_agentRelations.find(agentID)->second.relationshipStanding;
}


//-----------------------------------------------------------------------------------------------
void Faction::SetFactionStandingWithAgent(EntityID agentID, int standing)
{
   m_agentRelations[agentID].relationshipStanding = standing;
}


//-----------------------------------------------------------------------------------------------
int Faction::GetFactionStandingWithFaction(FactionID factionID) const
{
   return m_factionRelations.find(factionID)->second.relationshipStanding;
}


//-----------------------------------------------------------------------------------------------
void Faction::SetFactionStandingWithFaction(FactionID factionID, int standing)
{
   m_factionRelations[factionID].relationshipStanding = standing;
}


//-----------------------------------------------------------------------------------------------
void Faction::RemoveAgentRelationship(EntityID agentID)
{
   m_agentRelations.erase(agentID);
}


//-----------------------------------------------------------------------------------------------
void Faction::AdjustFactionStatus(Agent* instigator, FactionAction action)
{
   EntityID entityID = instigator->GetID();
   FactionID factionID = instigator->GetFactionID();

   switch (action)
   {
   case FA_TRIED_TO_ATTACK_ME:
   {
      m_agentRelations[entityID].relationshipStanding -= 3;
      m_factionRelations[factionID].relationshipStanding -= 1;
      break;
   }

   case FA_HIT_ME:
   {
      m_agentRelations[entityID].relationshipStanding -= 15;
      m_factionRelations[factionID].relationshipStanding -= 5;
      break;
   }

   case FA_TRIED_TO_HEAL_ME:
   {
      m_agentRelations[entityID].relationshipStanding += 5;
      m_factionRelations[factionID].relationshipStanding += 2;
      break;
   }

   case FA_HEALED_ME:
   {
      m_agentRelations[entityID].relationshipStanding += 10;
      m_factionRelations[factionID].relationshipStanding += 3;
      break;
   }
   }
}


//-----------------------------------------------------------------------------------------------
void Faction::WriteToXMLNode(XMLNode& agentNode) const
{
   XMLNode factionNode = agentNode.addChild("Factions");

   FactionRelationshipMap::const_iterator factionIter = m_factionRelations.begin();
   while (factionIter != m_factionRelations.end())
   {
      XMLNode factionRelationNode = factionNode.addChild("FactionRelationship");
      int id = factionIter->first;
      int standing = factionIter->second.relationshipStanding;

      char spareBuf[255];
      _itoa_s(id, spareBuf, 10);
      factionRelationNode.addAttribute("id", spareBuf);

      _itoa_s(standing, spareBuf, 10);
      factionRelationNode.addAttribute("standing", spareBuf);

      ++factionIter;
   }

   FactionRelationshipMap::const_iterator agentIter = m_agentRelations.begin();
   while (agentIter != m_agentRelations.end())
   {
      XMLNode agentRelationNode = factionNode.addChild("AgentRelationship");
      int id = agentIter->first;
      int standing = agentIter
         ->second.relationshipStanding;

      char spareBuf[255];
      _itoa_s(id, spareBuf, 10);
      agentRelationNode.addAttribute("id", spareBuf);

      _itoa_s(standing, spareBuf, 10);
      agentRelationNode.addAttribute("standing", spareBuf);

      ++agentIter;
   }
}


//-----------------------------------------------------------------------------------------------
void Faction::LoadFromXMLNode(const XMLNode& saveNode)
{
   XMLNode factions = saveNode.getChildNode("Factions");

   int factionRelationIndex = 0;
   XMLNode factionRelationNode = factions.getChildNode("FactionRelationship", &factionRelationIndex);
   while (!factionRelationNode.isEmpty())
   {
      FactionID idToAdjust = ReadXMLAttribute(factionRelationNode, "id", 0);
      int standing = ReadXMLAttribute(factionRelationNode, "standing", 0);
      SetFactionStandingWithFaction(idToAdjust, standing);

      factionRelationNode = factions.getChildNode("FactionRelationship", &factionRelationIndex);
   }

   int agentRelationIndex = 0;
   XMLNode agentRelationNode = factions.getChildNode("AgentRelationship", &agentRelationIndex);
   while (!agentRelationNode.isEmpty())
   {
      EntityID tempAgentID = ReadXMLAttribute(agentRelationNode, "id", 0);
      int standing = ReadXMLAttribute(agentRelationNode, "standing", 0);
      AddAgentRelation(tempAgentID, "", standing);

      agentRelationNode = factions.getChildNode("AgentRelationship", &agentRelationIndex);
   }
}


//-----------------------------------------------------------------------------------------------
STATIC std::string Faction::GetFactionStatusAsString(FactionStatus status)
{
   switch (status)
   {
   case FACTION_STATUS_DISLIKES:
   {
      return "dislikes";
   }

   case FACTION_STATUS_HATES:
   {
      return "hates";
   }

   case FACTION_STATUS_LIKES:
   {
      return "likes";
   }

   case FACTION_STATUS_LOVES:
   {
      return "loves";
   }
   }

   return "neutral";
}


//-----------------------------------------------------------------------------------------------
STATIC void Faction::LoadAllFactions()
{
   if (s_globalFactionMap == nullptr)
   {
      s_globalFactionMap = new FactionMap;
   }

   std::vector<std::string> factionFiles = EnumerateFilesInDirectory("Data/Factions", "*.Faction.xml");
   for (std::string& factionFile : factionFiles)
   {
      XMLNode factionFileNode = XMLNode::openFileHelper(factionFile.c_str(), "Factions");

      int factionIndex = 0;
      XMLNode factionNode = factionFileNode.getChildNode("Faction", &factionIndex);
      while (!factionNode.IsContentEmpty())
      {
         std::string factionName = factionNode.getAttribute("name");
         Faction* faction = CreateOrGetFactionByName(factionName);
         faction->PopulateFromXMLNode(factionNode);

         factionNode = factionFileNode.getChildNode("Faction", &factionIndex);
      }
   }

}


//-----------------------------------------------------------------------------------------------
STATIC void Faction::FreeAllFactions()
{
   FactionMapIter factionIter = s_globalFactionMap->begin();
   while (factionIter != s_globalFactionMap->end())
   {
      delete factionIter->second;
      ++factionIter;
   }

   delete s_globalFactionMap;
}


//-----------------------------------------------------------------------------------------------
STATIC Faction* Faction::CreateOrGetFactionByName(const std::string& name)
{
   FactionMapIter factionIter = s_globalFactionMap->find(name);

   if (factionIter != s_globalFactionMap->end())
   {
      return factionIter->second;
   }

   // Faction not in map; go ahead and make a new one
   Faction* newFaction = new Faction(name);
   FactionPair newFactionPair(name, newFaction);
   s_globalFactionMap->insert(newFactionPair);
   return newFaction;
}


//-----------------------------------------------------------------------------------------------
void Faction::ResolveEntityPointers(const std::map<int, Entity*>& loadedEntities)
{
   FactionRelationshipMap updatedMap;
   for (FactionRelationshipMap::iterator agentIter = m_agentRelations.begin(); agentIter != m_agentRelations.end(); ++agentIter)
   {
      int oldID = agentIter->first;
      int relationshipStanding = agentIter->second.relationshipStanding;
      std::string name = agentIter->second.name;
      Entity* replacementEntity = loadedEntities.at(oldID);
      FactionRelationship replacementRelationship = { relationshipStanding, replacementEntity->GetName() };
      updatedMap.insert(FactionRelationshipPair(replacementEntity->GetID(), replacementRelationship));
   }

   m_agentRelations = updatedMap;
}