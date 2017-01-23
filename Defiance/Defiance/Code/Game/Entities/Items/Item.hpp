#pragma once

#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Game/Core/GameCommon.hpp"
#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
enum ItemType
{
   INVALID_ITEM = -1,
   WEAPON_ITEM,
   ARMOR_ITEM,
   CONSUMABLE_ITEM,
   NUM_ITEM_TYPES,
};


//-----------------------------------------------------------------------------------------------
enum EquipmentSlot
{
   UNEQUIPPABLE = -1,
   PRIMARY_HAND_SLOT,
   SECONDARY_HAND_SLOT,
   HEAD_SLOT,
   CHEST_SLOT,
   ARMS_SLOT,
   LEGS_SLOT,
   NUM_EQUIPMENT_SLOTS,
};


//-----------------------------------------------------------------------------------------------
class Item
   : public Entity
{
public:
   Item(const XMLNode& blueprintNode);
   Item(const Item& copySource);
   Item(const Item& copySource, const XMLNode& node);

   virtual void AddToMap(Map* map, const TileCoords& position) override;
   void PickUp() { m_isPickedUp = true; }
   void SetDown() { m_isPickedUp = false; }

   virtual bool IsItem() const override { return true; }
   int GetAttackDamage() const { return m_attackDamage; }
   int GetDamageResistance() const { return m_damageResistance; }
   EquipmentSlot GetEquipmentSlot() const { return m_equipmentSlot; }
   ItemType GetItemType() const { return m_itemType; }
   bool IsSpeedBuff() const { return m_isSpeedBuff; }
   const IntRange& GetHealingPower() const { return m_healingPower; }
   char GetGlyph() const { return m_glyph; }
   bool IsPickedUp() const { return m_isPickedUp; }

   virtual bool WriteToXMLNode(XMLNode& entityNode) const override;
   static std::string GetStringForSlot(EquipmentSlot slot);
   static EquipmentSlot GetSlotForString(const std::string& slotString);
   static std::string GetStringForType(ItemType itemType);
   static ItemType GetTypeForString(const std::string& typeString);
   virtual void LoadFromXMLNode(const XMLNode& saveNode) override;

private:
   void PopulateFromXMLNode(const XMLNode& blueprintNode);
   void SetItemTypeFromString(const std::string& typeString);
   void SetEquipmentSlotFromString(const std::string& slotString);

   ItemType m_itemType;
   int m_attackDamage;
   int m_damageResistance;
   IntRange m_healingPower;
   bool m_isSpeedBuff;
   EquipmentSlot m_equipmentSlot;
   char m_glyph;
   bool m_isPickedUp;
};
typedef std::vector<Item*> Items;