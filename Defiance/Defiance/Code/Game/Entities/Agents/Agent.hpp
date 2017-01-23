#pragma once

#include <vector>
#include <map>
#include "Game/Core/GameCommon.hpp"
#include "Game/Entities/Entity.hpp"
#include "Game/Entities/Agents/Factions/Faction.hpp"
#include "Game/Entities/Items/Inventory.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
struct AttackData;
class Behavior;


//-----------------------------------------------------------------------------------------------
class Agent
   : public Entity
{
public:
   Agent();
   Agent(const XMLNode& blueprintNode);
   Agent(const Agent& copySource);
   Agent(const Agent& copySource, const XMLNode& node);
   Agent(int health, EntityType type, const TileCoords& position, char glyph, const Rgba& color, const Rgba& backgroundColor, const std::string& name);

   virtual ~Agent();
   
   void PopulateFromXMLNode(const XMLNode& blueprintNode);
   void PopulateFactionFromXMLNode(const XMLNode& factionNode);  
   void InitEquipment();

   virtual bool IsAgent() const override { return true; }
   virtual float Update();
   virtual void UpdateFOV();
   virtual void DereferenceEntity(Entity* entity) override;

   char GetGlyph() const { return m_glyph; }

   bool TestOneStepInDirection(TileDirection direction) const;
   bool MoveOneStepInDirection(TileDirection direction);

   bool TryGetItemFromCurrentTile();
   bool TryEquipNewItem(Item* newItem);

   int GetWeaponBonus() const;
   int GetArmorBonus() const;

   virtual void AddToMap(Map* map, const TileCoords& position) override;
   void MoveToPosition(const TileCoords& position);
   void AddVisibleAgent(float distanceToTile, Agent* agent);
   bool HasNeverMetAgent(EntityID agentID) const;
   bool HasNeverMetFaction(FactionID factionID) const;
   bool IsAgentVisible(Agent* agent) const;

   void AddVisibleItems(float distanceToTile, const Items& items);
   void AddVisibleFeature(float distanceToTile, Feature* feature);

   FactionID GetFactionID() const { return m_faction.GetFactionID(); }

   Agent* GetClosestEnemy();
   Agent* GetClosestAlly();
   Item* GetClosestVisibleItem();
   bool IsInventoryFull() const;
   void PerformAttack(AttackData& attackData);
   void ReceiveAttack(const AttackData& attackData);
   void AdjustFactionStatus(Agent* instigator, FactionAction action);

   virtual bool WriteToXMLNode(XMLNode& entityNode) const override;
   void WriteEquipmentToXMLNode(XMLNode& entityNode) const;
   void WriteBehaviorsToXMLNode(XMLNode& entityNode) const;

   virtual void LoadFromXMLNode(const XMLNode& saveNode) override;
   void LoadEquippedItemsFromXMLNode(const XMLNode& saveNode);

   virtual void ResolveEntityPointers(const std::map<int, Entity*>& loadedEntities) override;
   void ResolveEquipmentPointers(const std::map<int, Entity*>& loadedEntities);

protected:
   Faction m_faction;
   std::vector<Behavior*> m_behaviors;
   DistanceToAgentMap m_visibleAgents;
   DistanceToItemMap m_visibleItems;
   DistanceToFeatureMap m_visibleFeatures;
   Inventory m_inventory;
   Item* m_equippedItems[NUM_EQUIPMENT_SLOTS];
   char m_glyph;
};