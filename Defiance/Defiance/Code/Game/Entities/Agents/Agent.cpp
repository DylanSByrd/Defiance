#include "Engine/Parsers/XMLUtilities.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Core/GameContext.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Entities/Agents/Player.hpp"
#include "Game/Entities/Agents/Agent.hpp"
#include "Game/Entities/Items/Item.hpp"
#include "Game/Entities/Agents/Behaviors/Behavior.hpp"
#include "Game/Entities/Features/Feature.hpp"
#include "Game/FieldOfView/FieldOfViewAdvanced.hpp"
#include "Game/Combat/DefianceCombatSystem.hpp"
#include "Game/UI/GameMessageBox.hpp"


//-----------------------------------------------------------------------------------------------
Agent::Agent()
   : Entity()
   , m_glyph(' ')
{
   InitEquipment();
}


//-----------------------------------------------------------------------------------------------
Agent::Agent(const XMLNode& blueprintNode)
   : Entity(blueprintNode)
   , m_glyph(' ')
{
   InitEquipment();
   PopulateFromXMLNode(blueprintNode);
}


//-----------------------------------------------------------------------------------------------
Agent::Agent(const Agent& copySource)
   : Entity(copySource)
   , m_faction(copySource.m_faction)
   , m_inventory(copySource.m_inventory)
   , m_glyph(copySource.m_glyph)
{
   for (size_t slot = 0; slot < NUM_EQUIPMENT_SLOTS; ++slot)
   {
      m_equippedItems[slot] = copySource.m_equippedItems[slot];
   }

   for (Behavior* behaviorToCopy : copySource.m_behaviors)
   {
      Behavior* behavior = behaviorToCopy->Clone();
      behavior->SetOwningAgent(this);
      m_behaviors.push_back(behavior);
   }
}


//-----------------------------------------------------------------------------------------------
Agent::Agent(int health, EntityType type, const TileCoords& position, char glyph, const Rgba& color, const Rgba& backgroundColor, const std::string& name)
   :Entity(health, type, position, color, backgroundColor, name)
   , m_glyph(glyph)
{
   InitEquipment();
}


//-----------------------------------------------------------------------------------------------
Agent::Agent(const Agent& copySource, const XMLNode& node)
   : Agent(copySource)
{
   LoadFromXMLNode(node);
}


//-----------------------------------------------------------------------------------------------
Agent::~Agent()
{
   for (Behavior* behavior : m_behaviors)
   {
      delete behavior;
      behavior = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
void Agent::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
   std::string factionName = ReadXMLAttribute(blueprintNode, "faction", std::string(""));
   if (factionName.compare(""))
   {
      m_faction = *(Faction::CreateOrGetFactionByName(factionName));
   }

   XMLNode factionNode = blueprintNode.getChildNode("Faction");
   PopulateFactionFromXMLNode(factionNode);

   m_glyph = ReadXMLAttribute(blueprintNode, "glyph", m_glyph);
}


//-----------------------------------------------------------------------------------------------
void Agent::PopulateFactionFromXMLNode(const XMLNode& factionNode)
{
   if (factionNode.IsContentEmpty())
   {
      return;
   }

   std::string factionName = ReadXMLAttribute(factionNode, "name", std::string(""));
   if (!factionName.compare(""))
   {
      return;
   }

   m_faction = *(Faction::CreateOrGetFactionByName(factionName));
   m_faction.PopulateFromXMLNode(factionNode);
}


//-----------------------------------------------------------------------------------------------
void Agent::InitEquipment()
{
   for (size_t slot = 0; slot < NUM_EQUIPMENT_SLOTS; ++slot)
   {
      m_equippedItems[slot] = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
float Agent::Update()
{
   UpdateFOV();

   Behavior* behaviorToRun = nullptr;
   float currentBehaviorUtility = 0.f;

   for (Behavior* behavior : m_behaviors)
   {
      float thisBehaviorUtility = behavior->CalcUtility();
      if (thisBehaviorUtility > currentBehaviorUtility)
      {
         behaviorToRun = behavior;
         currentBehaviorUtility = thisBehaviorUtility;
      }
   }

   if (behaviorToRun != nullptr)
   {
      behaviorToRun->Run();
   }

   //#TODO
   return 1.f;
}


//-----------------------------------------------------------------------------------------------
void Agent::UpdateFOV()
{
   m_visibleAgents.clear();
   m_visibleItems.clear();
   m_visibleFeatures.clear();
   FieldOfViewAdvanced fov;
   fov.CalculateFieldOfViewForAgent(this, 20, m_gameMap, false);
}


//-----------------------------------------------------------------------------------------------
void Agent::DereferenceEntity(Entity* entity)
{
   if (entity->IsAgent())
   {
      m_faction.RemoveAgentRelationship(entity->GetID());

      for (DistanceToAgentIter agentIter = m_visibleAgents.begin(); agentIter != m_visibleAgents.end(); ++agentIter)
      {
         if (entity == agentIter->second)
         {
            m_visibleAgents.erase(agentIter);
            break;
         }
      }
   }
   else if (entity->IsItem())
   {
      for (DistanceToItemIter itemIter = m_visibleItems.begin(); itemIter != m_visibleItems.end(); ++itemIter)
      {
         if (entity == itemIter->second)
         {
            m_visibleItems.erase(itemIter);
            break;
         }
      }

      m_inventory.DereferenceItem(entity);
   }
   else  //(entity->IsFeature())
   {
      for (DistanceToFeatureIter featureIter = m_visibleFeatures.begin(); featureIter != m_visibleFeatures.end(); ++featureIter)
      {
         if (entity == featureIter->second)
         {
            m_visibleFeatures.erase(featureIter);
            break;
         }
      }
   }


   for (Behavior* behavior : m_behaviors)
   {
      behavior->DereferenceEntity(entity);
   }
}


//-----------------------------------------------------------------------------------------------
bool Agent::TestOneStepInDirection(TileDirection direction) const
{
   bool canMoveInDirection = true;
   const Tile& testTile = m_gameMap->GetTileInDirection(m_position, direction);
   if (testTile.type == STONE_TYPE || testTile.type == INVALID_TYLE_TYPE || testTile.IsOccupiedByAgent()
      || testTile.DoesBlockPathing())
   {
      return !canMoveInDirection;
   }

   return canMoveInDirection;
}


//-----------------------------------------------------------------------------------------------
bool Agent::MoveOneStepInDirection(TileDirection direction)
{
   bool didMove = true;

   if (!TestOneStepInDirection(direction))
   {
      return !didMove;
   }

   const TileCoords& newPosition = m_gameMap->GetTileCoordsInDirection(m_position, direction);
   MoveToPosition(newPosition);

   if (IsPlayer())
   {
      const Tile* newTile = m_gameMap->GetTileAtTileCoords(newPosition);
      newTile->PrintItemInfo();
   }

   return true;
}



//-----------------------------------------------------------------------------------------------
bool Agent::TryGetItemFromCurrentTile()
{
   bool didPickUpNewItem = true;

   Tile* currentTile = m_gameMap->GetTileAtTileCoords(m_position);

   if (!currentTile->HasItems())
   {
      return !didPickUpNewItem;
   }

   Item* firstItemOnTile = nullptr;
   currentTile->inventory.GetItem(&firstItemOnTile);

   bool didEquip = TryEquipNewItem(firstItemOnTile);
   if (!didEquip)
   {
      if (m_inventory.IsInventoryFull())
      {
         if (IsPlayer())
         {
            currentTile->inventory.AddItem(firstItemOnTile);
            g_theGameMessageBox->PrintDangerMessage("Your inventory is full!");
         }
         return !didPickUpNewItem;
      }

      firstItemOnTile->PickUp();
      m_inventory.AddItem(firstItemOnTile);

      if (IsPlayer())
      {
         g_theGameMessageBox->PrintGoodMessage(Stringf("You put the %s in your backpack.", firstItemOnTile->GetName().c_str()));
         g_theGameMessageBox->PrintNeutralMessage(Stringf("You have %d slot(s) remaining in your inventory.", m_inventory.GetNumInventorySlotsRemaining()));
      }
      else
      {
         const GameContext* context = g_theGame->GetGameContext();
         const Player* player = context->activePlayer;
         if (player != nullptr
            && player->IsAgentVisible(this))
         {
            g_theGameMessageBox->PrintNeutralMessage(Stringf("The %s picks up a %s.", m_name.c_str(), firstItemOnTile->GetName().c_str()));
         }
      }
   }

   return didPickUpNewItem;
}


//-----------------------------------------------------------------------------------------------
bool Agent::TryEquipNewItem(Item* newItem)
{
   bool didEquipItem = true;
   EquipmentSlot slot = newItem->GetEquipmentSlot();
   if (slot == UNEQUIPPABLE)
   {
      return !didEquipItem;
   }

   Item* oldItemInSlot = m_equippedItems[slot];

   if (oldItemInSlot == nullptr)
   {
      m_equippedItems[slot] = newItem;
      newItem->PickUp();

      if (IsPlayer())
      {
         g_theGameMessageBox->PrintGoodMessage(Stringf("You equip the %s.", newItem->GetName().c_str()));
      }
      else
      {
         const GameContext* context = g_theGame->GetGameContext();
         const Player* player = context->activePlayer;
         if (player != nullptr
            && player->IsAgentVisible(this))
         {
            g_theGameMessageBox->PrintCautionMessage(Stringf("The %s equips a %s!", m_name.c_str(), newItem->GetName().c_str()));
         }
      }
      return didEquipItem;
   }

   if ((newItem->GetItemType() == WEAPON_ITEM
      && newItem->GetAttackDamage() > oldItemInSlot->GetAttackDamage())
      || (newItem->GetItemType() == ARMOR_ITEM
         && newItem->GetDamageResistance() > oldItemInSlot->GetDamageResistance()))
   {
      m_equippedItems[slot] = newItem;
      newItem->PickUp();

      if (m_inventory.IsInventoryFull())
      {
         Tile* currentTile = m_gameMap->GetTileAtTileCoords(m_position);
         currentTile->inventory.AddItem(oldItemInSlot);
         oldItemInSlot->SetDown();

         if (IsPlayer())
         {
            g_theGameMessageBox->PrintGoodMessage(Stringf("You equip the superior %s and drop the %s as your inventory is full.", newItem->GetName().c_str(), oldItemInSlot->GetName().c_str()));
         }
         else
         {
            const GameContext* context = g_theGame->GetGameContext();
            const Player* player = context->activePlayer;
            if (player != nullptr
               && player->IsAgentVisible(this))
            {
               g_theGameMessageBox->PrintCautionMessage(Stringf("The %s equips a %s, dropping a %s!", m_name.c_str(), newItem->GetName().c_str(), oldItemInSlot->GetName().c_str()));
            }
         }
         return didEquipItem;
      }

      m_inventory.AddItem(oldItemInSlot);

      if (IsPlayer())
      {
         g_theGameMessageBox->PrintGoodMessage(Stringf("You equip the superior %s and put the %s in your backpack.", newItem->GetName().c_str(), oldItemInSlot->GetName().c_str()));
      }
      else
      {
         const GameContext* context = g_theGame->GetGameContext();
         const Player* player = context->activePlayer;
         if (player != nullptr
            && player->IsAgentVisible(this))
         {
            g_theGameMessageBox->PrintCautionMessage(Stringf("The %s equips a %s!", m_name.c_str(), newItem->GetName().c_str()));
         }
      }

      return didEquipItem;
   }

   return !didEquipItem;
}


//-----------------------------------------------------------------------------------------------
int Agent::GetWeaponBonus() const
{
   int damageBonus = 0;
   if (m_equippedItems[PRIMARY_HAND_SLOT] == nullptr)
   {
      return damageBonus;
   }

   damageBonus = m_equippedItems[PRIMARY_HAND_SLOT]->GetAttackDamage();
   return damageBonus;
}


//-----------------------------------------------------------------------------------------------
int Agent::GetArmorBonus() const
{
   int armorBonus = 0;

   for (size_t slot = 0; slot < NUM_EQUIPMENT_SLOTS; ++slot)
   {
      if (slot != PRIMARY_HAND_SLOT
         && m_equippedItems[slot] != nullptr)
      {
         armorBonus += m_equippedItems[slot]->GetDamageResistance();
      }
   }

   return armorBonus;
}


//-----------------------------------------------------------------------------------------------
void Agent::AddToMap(Map* map, const TileCoords& position)
{
   m_gameMap = map;
   MoveToPosition(position);
}


//-----------------------------------------------------------------------------------------------
void Agent::MoveToPosition(const TileCoords& position)
{
   Tile* tileToOccupy = m_gameMap->GetTileAtTileCoords(position);
   ASSERT_OR_DIE(!tileToOccupy->IsOccupiedByAgent(),
      Stringf("Defiance ERROR: Tried to add entity %s with id %d to position %f,%f; position is occupied!", m_name.c_str(), m_ID, position.x, position.y));

   Tile* currentTile = m_gameMap->GetTileAtTileCoords(m_position);
   currentTile->RemoveAgent();

   m_position = position;
   tileToOccupy->occupyingAgent = this;
}


//-----------------------------------------------------------------------------------------------
void Agent::AddVisibleAgent(float distanceToTile, Agent* agent)
{
   m_visibleAgents.insert(DistanceToAgentPair(distanceToTile, agent));

   if (HasNeverMetAgent(agent->m_ID))
   {
      FactionID agentFactionID = agent->m_faction.GetFactionID();
      if (HasNeverMetFaction(agentFactionID))
      {
         std::string factionName = agent->m_faction.GetName();
         m_faction.AddFactionRelation(agentFactionID, factionName, FACTION_STATUS_NEUTRAL);
      }

      int factionStanding = m_faction.GetFactionStandingWithFaction(agentFactionID);
      m_faction.AddAgentRelation(agent->m_ID, agent->m_name, factionStanding);
   }
}


//-----------------------------------------------------------------------------------------------
bool Agent::HasNeverMetAgent(EntityID agentID) const
{
   return !m_faction.IsAgentRelationPresent(agentID);
}


//-----------------------------------------------------------------------------------------------
bool Agent::HasNeverMetFaction(FactionID factionID) const
{
   return !m_faction.IsFactionRelationPresent(factionID);
}


//-----------------------------------------------------------------------------------------------
bool Agent::IsAgentVisible(Agent* agent) const
{
   bool agentIsVisible = true;

   if (agent == this)
   {
      return agentIsVisible;
   }

   for (DistanceToAgentConstIter agentIter = m_visibleAgents.begin(); agentIter != m_visibleAgents.end(); ++agentIter)
   {
      if (agent == agentIter->second)
      {
         return agentIsVisible;
      }
   }

   return !agentIsVisible;
}


//-----------------------------------------------------------------------------------------------
void Agent::AddVisibleItems(float distanceToTile, const Items& items)
{
   for (Item* item : items)
   {
      m_visibleItems.insert(DistanceToItemPair(distanceToTile, item));
   }
}


//-----------------------------------------------------------------------------------------------
void Agent::AddVisibleFeature(float distanceToTile, Feature* feature)
{
   m_visibleFeatures.insert(DistanceToFeaturePair(distanceToTile, feature));
}


//-----------------------------------------------------------------------------------------------
Agent* Agent::GetClosestEnemy()
{
   for (DistanceToAgentIter agentIter = m_visibleAgents.begin(); agentIter != m_visibleAgents.end(); ++agentIter)
   {
      Agent* currentAgent = agentIter->second;
      EntityID agentID = currentAgent->m_ID;

      int factionStandingWithAgent = m_faction.GetFactionStandingWithAgent(agentID);
      if (factionStandingWithAgent <= FACTION_STATUS_DISLIKES)
      {
         return currentAgent;
      }
   }

   // No enemies found
   return nullptr;
}


//-----------------------------------------------------------------------------------------------
Agent* Agent::GetClosestAlly()
{
   for (DistanceToAgentIter agentIter = m_visibleAgents.begin(); agentIter != m_visibleAgents.end(); ++agentIter)
   {
      Agent* currentAgent = agentIter->second;
      EntityID agentID = currentAgent->m_ID;

      int factionStandingWithAgent = m_faction.GetFactionStandingWithAgent(agentID);
      if (factionStandingWithAgent >= FACTION_STATUS_LIKES)
      {
         return currentAgent;
      }
   }

   // No allies found
   return nullptr;
}


//-----------------------------------------------------------------------------------------------
Item* Agent::GetClosestVisibleItem()
{
   if (m_visibleItems.size() == 0)
   {
      return nullptr;
   }

   return m_visibleItems.begin()->second;
}


//-----------------------------------------------------------------------------------------------
bool Agent::IsInventoryFull() const
{
   return m_inventory.IsInventoryFull();
}


//-----------------------------------------------------------------------------------------------
void Agent::PerformAttack(AttackData& attackData)
{
   DefianceCombatSystem::PerformAttack(attackData);
}


//-----------------------------------------------------------------------------------------------
void Agent::ReceiveAttack(const AttackData& attackData)
{
   Agent* attacker = attackData.instigator;

   switch (attackData.result)
   {
   case ATTACK_RESULT_ATTACK_MISS:
   {
      AdjustFactionStatus(attacker, FA_TRIED_TO_ATTACK_ME);
   }
   
   case ATTACK_RESULT_HIT:
   {
      AdjustFactionStatus(attacker, FA_HIT_ME);
   }
   }

   if (attackData.didHit)
   {
      m_currentHealth -= attackData.damageDealt;
      m_currentHealth = Clampi(m_currentHealth, 0, m_maxHealth);
   }
}


//-----------------------------------------------------------------------------------------------
void Agent::AdjustFactionStatus(Agent* instigator, FactionAction action)
{
   m_faction.AdjustFactionStatus(instigator, action);
}


//-----------------------------------------------------------------------------------------------
bool Agent::WriteToXMLNode(XMLNode& entityNode) const
{
   bool writeSuccessful = true;

   if (!Entity::WriteToXMLNode(entityNode))
   {
      return !writeSuccessful;
   }

   m_faction.WriteToXMLNode(entityNode);
   m_inventory.WriteToXMLNode(entityNode);
   WriteEquipmentToXMLNode(entityNode);
   WriteBehaviorsToXMLNode(entityNode);

   return writeSuccessful;
}


//-----------------------------------------------------------------------------------------------
void Agent::WriteEquipmentToXMLNode(XMLNode& entityNode) const
{
   XMLNode equipmentNode = entityNode.addChild("Equipment");
   for (size_t slotIndex = 0; slotIndex < NUM_EQUIPMENT_SLOTS; ++slotIndex)
   {
      if (m_equippedItems[slotIndex] != nullptr)
      {
         Item* equippedItem = m_equippedItems[slotIndex];

         char spareBuf[255];
         _itoa_s(equippedItem->GetID(), spareBuf, 10);

         XMLNode slotNode = equipmentNode.addChild("EquipmentSlot");
         slotNode.addAttribute("slot", Item::GetStringForSlot((EquipmentSlot)slotIndex).c_str());
         slotNode.addAttribute("id", spareBuf);
      }
   }
}


//-----------------------------------------------------------------------------------------------
void Agent::WriteBehaviorsToXMLNode(XMLNode& entityNode) const
{
   XMLNode behaviorNode = entityNode.addChild("Behaviors");

   for (Behavior* behavior : m_behaviors)
   {
      behavior->WriteToXMLNode(behaviorNode);
   }
}


//-----------------------------------------------------------------------------------------------
void Agent::LoadFromXMLNode(const XMLNode& saveNode)
{
   Entity::LoadFromXMLNode(saveNode);

   m_faction.LoadFromXMLNode(saveNode);

   // load all behaviors

   m_inventory.LoadFromXMLNode(saveNode);

   LoadEquippedItemsFromXMLNode(saveNode);
}


//-----------------------------------------------------------------------------------------------
void Agent::LoadEquippedItemsFromXMLNode(const XMLNode& saveNode)
{
   XMLNode equipmentNode = saveNode.getChildNode("Equipment");

   int slotIndex = 0;
   XMLNode slotNode = equipmentNode.getChildNode("EquipmentSlot", &slotIndex);

   while (!slotNode.isEmpty())
   {
      std::string slotAsString = ReadXMLAttribute(slotNode, "slot", std::string("unequippable"));
      EquipmentSlot slot = Item::GetSlotForString(slotAsString);

      int tempID = ReadXMLAttribute(slotNode, "id", 0);
      m_equippedItems[slot] = (Item*)tempID;

      slotNode = equipmentNode.getChildNode("EquipmentSlot", &slotIndex);
   }
}


//-----------------------------------------------------------------------------------------------
void Agent::ResolveEntityPointers(const std::map<int, Entity*>& loadedEntities)
{
   // behaviors
   // faction
   ResolveEquipmentPointers(loadedEntities);
   m_inventory.ResolveEntityPointers(loadedEntities);
}


//-----------------------------------------------------------------------------------------------
void Agent::ResolveEquipmentPointers(const std::map<int, Entity*>& loadedEntities)
{
   for (size_t slotIndex = 0; slotIndex < NUM_EQUIPMENT_SLOTS; ++slotIndex)
   {
      if (m_equippedItems[slotIndex] != nullptr)
      {
         int oldID = (int)m_equippedItems[slotIndex];
         m_equippedItems[slotIndex] = (Item*)loadedEntities.at(oldID);
      }
   }
}