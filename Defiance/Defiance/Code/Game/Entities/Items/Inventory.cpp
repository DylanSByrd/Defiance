#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Items/Inventory.hpp"
#include "Game/UI/GameMessageBox.hpp"


//-----------------------------------------------------------------------------------------------
STATIC int Inventory::INVENTORY_SIZE_LIMIT = 5;


//-----------------------------------------------------------------------------------------------
bool Inventory::AddItem(Item* itemToAdd)
{
   bool itemAddedSuccessfully = true;

   if (IsInventoryFull())
   {
      return !itemAddedSuccessfully;
   }

   ItemType typeToAdd = itemToAdd->GetItemType();
   m_items[typeToAdd].push_back(itemToAdd);
   return itemAddedSuccessfully;
}


//-----------------------------------------------------------------------------------------------
bool Inventory::GetItem(Item** outItem)
{
   bool itemTakenSuccessfully = true;

   // find first available item
   for (size_t itemType = 0; itemType < NUM_ITEM_TYPES; ++itemType)
   {
      if (GetItemOfType((ItemType)itemType, outItem))
      {
         return itemTakenSuccessfully;
      }
   }

   // no items
   return !itemTakenSuccessfully;
}


//-----------------------------------------------------------------------------------------------
bool Inventory::GetItemOfType(ItemType type, Item** outItem)
{
   bool itemRetrievedSuccessfully = true;

   if (m_items[type].size() == 0)
   {
      return !itemRetrievedSuccessfully;
   }

   // #TODO - allow agent to "pick" an item instead of using the first one
   *outItem = m_items[type].front();
   m_items[type].erase(m_items[type].begin());
   return itemRetrievedSuccessfully;
}


//-----------------------------------------------------------------------------------------------
void Inventory::DereferenceItem(Entity* itemToRemove)
{
   for (size_t typeIndex = 0; typeIndex < NUM_ITEM_TYPES; ++typeIndex)
   {
      for (Items::iterator itemIter = m_items[typeIndex].begin(); itemIter != m_items[typeIndex].end(); ++itemIter)
      {
         if (*itemIter == itemToRemove)
         {
            m_items[typeIndex].erase(itemIter);
            return;
         }
      }
   }
}


//-----------------------------------------------------------------------------------------------
Items Inventory::GetAllItems() const
{
   Items itemReferences;
   itemReferences.reserve(GetNumItemsInInventory());

   for (size_t typeIndex = 0; typeIndex < NUM_ITEM_TYPES; ++typeIndex)
   {
      for (Item* item : m_items[typeIndex])
      {
         itemReferences.push_back(item);
      }
   }

   return itemReferences;
}


//-----------------------------------------------------------------------------------------------
bool Inventory::IsInventoryFull() const
{
   return (GetNumInventorySlotsRemaining() == 0);
}


//-----------------------------------------------------------------------------------------------
int Inventory::GetNumInventorySlotsRemaining() const
{
   return (INVENTORY_SIZE_LIMIT - GetNumItemsInInventory());
}


//-----------------------------------------------------------------------------------------------
int Inventory::GetNumItemsInInventory() const
{
   int itemCount = 0;

   for (size_t typeIndex = 0; typeIndex < NUM_ITEM_TYPES; ++typeIndex)
   {
      itemCount += m_items[typeIndex].size();
   }

   return itemCount;
}


//-----------------------------------------------------------------------------------------------
char Inventory::GetItemGlyph() const
{
   for (size_t typeIndex = 0; typeIndex < NUM_ITEM_TYPES; ++typeIndex)
   {
      Items items = m_items[typeIndex];

      for (Item* item : items)
      {
         return item->GetGlyph();
      }
   }

   return ' ';
}


//-----------------------------------------------------------------------------------------------
void Inventory::PrintItemInfo() const
{
   for (size_t typeIndex = 0; typeIndex < NUM_ITEM_TYPES; ++typeIndex)
   {
      Items items = m_items[typeIndex];

      for (Item* item : items)
      {
         g_theGameMessageBox->PrintNeutralMessage(Stringf("You find a %s here.", item->GetName().c_str()));
      }
   }
}


//-----------------------------------------------------------------------------------------------
void Inventory::WriteToXMLNode(XMLNode& agentNode) const
{
   XMLNode inventoryNode = agentNode.addChild("Inventory");

   for (size_t typeIndex = 0; typeIndex < NUM_ITEM_TYPES; ++typeIndex)
   {
      Items items = m_items[typeIndex];

      for (Item* item : items)
      {
         XMLNode itemNode = inventoryNode.addChild("Item");

         char spareBuf[255];
         _itoa_s(item->GetID(), spareBuf, 10);

         itemNode.addAttribute("id", spareBuf);
         itemNode.addAttribute("type", item->GetStringForType(item->GetItemType()).c_str());
      }
   }
}


//-----------------------------------------------------------------------------------------------
void Inventory::LoadFromXMLNode(const XMLNode& saveNode)
{
   XMLNode inventoryNode = saveNode.getChildNode("Inventory");

   int itemIndex = 0;
   XMLNode itemNode = inventoryNode.getChildNode("Item", &itemIndex);
   while (!itemNode.isEmpty())
   {
      EntityID tempID = ReadXMLAttribute(itemNode, "id", 0);
      ItemType type = Item::GetTypeForString(ReadXMLAttribute(itemNode, "type", std::string("invalid")));

      m_items[type].push_back((Item*)tempID);


      itemNode = inventoryNode.getChildNode("Item", &itemIndex);
   }
}


//-----------------------------------------------------------------------------------------------
void Inventory::ResolveEntityPointers(const std::map<int, Entity*>& loadedEntities)
{
   for (size_t typeIndex = 0; typeIndex < NUM_ITEM_TYPES; ++typeIndex)
   {
      Items items = m_items[typeIndex];

      for (Items::iterator itemIter = m_items[typeIndex].begin(); itemIter != m_items[typeIndex].end(); ++itemIter)
      {
         int oldID = (int)(*itemIter);
         *itemIter = (Item*)loadedEntities.at(oldID);
      }
   }
}
