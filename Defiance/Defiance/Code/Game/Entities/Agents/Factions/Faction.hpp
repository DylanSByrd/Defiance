#pragma once

#include <string>
#include <map>
#include "Game/Core/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class Entity;


//-----------------------------------------------------------------------------------------------
enum FactionStatus
{
   FACTION_STATUS_HATES = -20,
   FACTION_STATUS_DISLIKES = -10,
   FACTION_STATUS_NEUTRAL = 0,
   FACTION_STATUS_LIKES = 10,
   FACTION_STATUS_LOVES = 20,
};


//-----------------------------------------------------------------------------------------------
enum FactionAction
{
   FA_TRIED_TO_ATTACK_ME,
   FA_HIT_ME,
   FA_TRIED_TO_HEAL_ME,
   FA_HEALED_ME,
   NUM_FACTION_ACTIONS,
};


//-----------------------------------------------------------------------------------------------
struct FactionRelationship
{
   int relationshipStanding;
   std::string name;
};
typedef std::map<int, FactionRelationship> FactionRelationshipMap;
typedef std::pair<int, FactionRelationship> FactionRelationshipPair;


//-----------------------------------------------------------------------------------------------
class Faction;
typedef std::map<std::string, Faction*> FactionMap;
typedef std::pair<std::string, Faction*> FactionPair;
typedef FactionMap::iterator FactionMapIter;


//-----------------------------------------------------------------------------------------------
class Faction
{
public:
   Faction();

   void PopulateFromXMLNode(const XMLNode& factionNode);
   void PopulateGivenStatusFromNode(const XMLNode& factionNode, FactionStatus status);

   const std::string& GetName() const { return m_name; }
   FactionID GetFactionID() const { return m_factionID; }

   bool IsAgentRelationPresent(EntityID agentID) const;
   bool IsFactionRelationPresent(FactionID factionID) const;
   void AddAgentRelation(EntityID agentID, const std::string& name, int standing);
   void AddFactionRelation(FactionID, const std::string& name, int standing);
   int GetFactionStandingWithAgent(EntityID agentID) const;
   void SetFactionStandingWithAgent(EntityID agentID, int standing);
   int GetFactionStandingWithFaction(FactionID factionID) const;
   void SetFactionStandingWithFaction(FactionID factionID, int standing);
   void RemoveAgentRelationship(EntityID agentID);
   void AdjustFactionStatus(Agent* instigator, FactionAction action);

   void WriteToXMLNode(XMLNode& agentNode) const;
   void LoadFromXMLNode(const XMLNode& saveNode);
   void ResolveEntityPointers(const std::map<int, Entity*>& loadedEntities);

   static std::string GetFactionStatusAsString(FactionStatus status);
   static void LoadAllFactions();
   static void FreeAllFactions();
   static Faction* CreateOrGetFactionByName(const std::string& name);
   static FactionMap* s_globalFactionMap;

   static int s_nextID;
   static const int INVALID_FACTION_ID;

private:
   Faction(const std::string& name);

   std::string m_name;
   FactionID m_factionID;
   FactionRelationshipMap m_factionRelations;
   FactionRelationshipMap m_agentRelations;   
};