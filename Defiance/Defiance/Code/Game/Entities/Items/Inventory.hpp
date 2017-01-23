#pragma once

#include "Game/Entities/Items/Item.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
class Inventory
{
public:
   bool AddItem(Item* itemToAdd);
   bool GetItem(Item** outItem);
   bool GetItemOfType(ItemType type, Item** outItem);
   void DereferenceItem(Entity* itemToRemove);

   Items GetAllItems() const;
   bool IsInventoryFull() const;
   int GetNumInventorySlotsRemaining() const;
   int GetNumItemsInInventory() const;
   int GetNumItemsOfTypeInInventory(ItemType type) const { return m_items[type].size(); }
   char GetItemGlyph() const;
   void PrintItemInfo() const;

   void WriteToXMLNode(XMLNode& agentNode) const;
   void LoadFromXMLNode(const XMLNode& saveNode);
   void ResolveEntityPointers(const std::map<int, Entity*>& loadedEntities);


   static int INVENTORY_SIZE_LIMIT;

private:
   Items m_items[NUM_ITEM_TYPES];
};