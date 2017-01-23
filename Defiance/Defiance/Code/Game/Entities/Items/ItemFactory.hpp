#pragma once

#include <map>
#include <string>
#include "Game/Entities/Items/Item.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class ItemFactory;
typedef std::map<std::string, ItemFactory*> ItemFactoryMap;


//-----------------------------------------------------------------------------------------------
class ItemFactory
{
public:
   ItemFactory(const XMLNode& itemBlueprintNode);
   ~ItemFactory();

   Item* CreateItem() const;
   Item* CreateFromNode(const XMLNode& node) const;

   static void LoadAllItemBlueprints();
   static void FreeAllItemBlueprints();
   static Item* CreateItemFromXMLNode(const XMLNode& node);
   static Item* CreateByNameOfType(const std::string& name, ItemType type);
   static Item* CreateRandomItem();
   static Item* CreateRandomItemOfType(ItemType type);
   static ItemFactoryMap* s_factoryMaps[NUM_ITEM_TYPES];

private:
   void PopulateFromXMLNode(const XMLNode& itemBlueprintNode);

   Item* m_blueprint;
};
typedef std::pair<std::string, ItemFactory*> ItemFactoryPair;
typedef ItemFactoryMap::iterator ItemFactoryIter;