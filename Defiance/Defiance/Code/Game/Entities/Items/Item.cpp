#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Items/Item.hpp"
#include "Game/Map/Map.hpp"


//-----------------------------------------------------------------------------------------------
Item::Item(const XMLNode& blueprintNode)
   : Entity(blueprintNode)
   , m_itemType(INVALID_ITEM)
   , m_equipmentSlot(UNEQUIPPABLE)
   , m_isPickedUp(false)
{
   PopulateFromXMLNode(blueprintNode);
}


//-----------------------------------------------------------------------------------------------
Item::Item(const Item& copySource)
   : Entity(copySource)
   , m_itemType(copySource.m_itemType)
   , m_attackDamage(copySource.m_attackDamage)
   , m_damageResistance(copySource.m_damageResistance)
   , m_healingPower(copySource.m_healingPower)
   , m_isSpeedBuff(copySource.m_isSpeedBuff)
   , m_equipmentSlot(copySource.m_equipmentSlot)
   , m_glyph(copySource.m_glyph)
   , m_isPickedUp(copySource.m_isPickedUp)
{}


//-----------------------------------------------------------------------------------------------
Item::Item(const Item& copySource, const XMLNode& node)
   : Item(copySource)
{
   LoadFromXMLNode(node);
}


//-----------------------------------------------------------------------------------------------
void Item::AddToMap(Map* map, const TileCoords& position)
{
   m_gameMap = map;
   m_position = position;
   map->GetTileAtTileCoords(position)->inventory.AddItem(this);
}


//-----------------------------------------------------------------------------------------------
bool Item::WriteToXMLNode(XMLNode& entityNode) const
{
   bool writeSuccessful = true;

   XMLNode itemNode = entityNode.addChild("ItemBlueprint");
   if (!Entity::WriteToXMLNode(itemNode))
   {
      return !writeSuccessful;
   }

   if (m_isPickedUp)
   {
      itemNode.addAttribute("isPickedUp", "true");
   }

   return writeSuccessful;
}


//-----------------------------------------------------------------------------------------------
std::string Item::GetStringForSlot(EquipmentSlot slot)
{
   switch (slot)
   {
   case PRIMARY_HAND_SLOT: return "primaryHand";
   case SECONDARY_HAND_SLOT: return "secondaryHand";
   case HEAD_SLOT: return "head";
   case CHEST_SLOT: return "chest";
   case ARMS_SLOT: return "arms";
   case LEGS_SLOT: return "legs";
   }

   return "unequippable";
}


//-----------------------------------------------------------------------------------------------
EquipmentSlot Item::GetSlotForString(const std::string& slotString)
{
   if (!strcmp(slotString.c_str(), "primaryHand"))
   {
      return PRIMARY_HAND_SLOT;
   }

   if (!strcmp(slotString.c_str(), "secondaryHand"))
   {
      return SECONDARY_HAND_SLOT;
   }

   if (!strcmp(slotString.c_str(), "head"))
   {
      return HEAD_SLOT;
   }

   if (!strcmp(slotString.c_str(), "chest"))
   {
      return CHEST_SLOT;
   }

   if (!strcmp(slotString.c_str(), "arms"))
   {
      return ARMS_SLOT;
   }

   if (!strcmp(slotString.c_str(), "legs"))
   {
      return LEGS_SLOT;
   }

   return UNEQUIPPABLE;
}


//-----------------------------------------------------------------------------------------------
STATIC std::string Item::GetStringForType(ItemType itemType)
{
   switch (itemType)
   {
   case WEAPON_ITEM: return "weapon";
   case ARMOR_ITEM: return "armor";
   case CONSUMABLE_ITEM: return "consumable";
   }

   return "invalid";
}


//-----------------------------------------------------------------------------------------------
STATIC ItemType Item::GetTypeForString(const std::string& typeString)
{
   if (!strcmp(typeString.c_str(), "weapon"))
   {
      return WEAPON_ITEM;
   }

   if (!strcmp(typeString.c_str(), "armor"))
   {
      return ARMOR_ITEM;
   }

   if (!strcmp(typeString.c_str(), "consumable")
      || !strcmp(typeString.c_str(), "potion"))
   {
      return CONSUMABLE_ITEM;
   }

   return INVALID_ITEM;
}


//-----------------------------------------------------------------------------------------------
void Item::LoadFromXMLNode(const XMLNode& saveNode)
{
   Entity::LoadFromXMLNode(saveNode);

   m_isPickedUp = ReadXMLAttribute(saveNode, "isPickedUp", false);
}


//-----------------------------------------------------------------------------------------------
void Item::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
   m_attackDamage = ReadXMLAttribute(blueprintNode, "damage", 0);
   m_damageResistance = ReadXMLAttribute(blueprintNode, "armor", 0);
   m_healingPower = ReadXMLAttribute(blueprintNode, "healingPower", IntRange::ZERO);
   m_isSpeedBuff = ReadXMLAttribute(blueprintNode, "isSpeedBuff", false);

   SetItemTypeFromString(ReadXMLAttribute(blueprintNode, "type", std::string("invalid")));
   SetEquipmentSlotFromString(ReadXMLAttribute(blueprintNode, "slot", std::string("unequippable")));
}


//-----------------------------------------------------------------------------------------------
void Item::SetItemTypeFromString(const std::string& typeString)
{
   if (!strcmp(typeString.c_str(), "invalid"))
   {
      m_itemType = INVALID_ITEM;
      m_glyph = '?';
   }

   if (!strcmp(typeString.c_str(), "weapon"))
   {
      m_itemType = WEAPON_ITEM;
      m_glyph = '(';
   }

   if (!strcmp(typeString.c_str(), "armor"))
   {
      m_itemType = ARMOR_ITEM;
      m_glyph = '[';
   }

   if (!strcmp(typeString.c_str(), "consumable")
      || !strcmp(typeString.c_str(), "potion"))
   {
      m_itemType = CONSUMABLE_ITEM;
      m_glyph = '!';
   }
}


//-----------------------------------------------------------------------------------------------
void Item::SetEquipmentSlotFromString(const std::string& slotString)
{
   if (!strcmp(slotString.c_str(), "unequippable"))
   {
      m_equipmentSlot = UNEQUIPPABLE;
   }

   if (!strcmp(slotString.c_str(), "primaryHand"))
   {
      m_equipmentSlot = PRIMARY_HAND_SLOT;
   }
   
   if (!strcmp(slotString.c_str(), "secondaryHand"))
   {
      m_equipmentSlot = SECONDARY_HAND_SLOT;
   }
   
   if (!strcmp(slotString.c_str(), "head"))
   {
      m_equipmentSlot = HEAD_SLOT;
   }
   
   if (!strcmp(slotString.c_str(), "chest"))
   {
      m_equipmentSlot = CHEST_SLOT;
   }
   
   if (!strcmp(slotString.c_str(), "arms"))
   {
      m_equipmentSlot = ARMS_SLOT;
   }

   if (!strcmp(slotString.c_str(), "legs"))
   {
      m_equipmentSlot = LEGS_SLOT;
   }
}