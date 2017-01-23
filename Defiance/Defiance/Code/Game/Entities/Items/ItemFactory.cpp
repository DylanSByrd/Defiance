#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/IO/FileUtils.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Entities/Items/ItemFactory.hpp"


//-----------------------------------------------------------------------------------------------
STATIC ItemFactoryMap* ItemFactory::s_factoryMaps[] = { nullptr };


//-----------------------------------------------------------------------------------------------
ItemFactory::ItemFactory(const XMLNode& itemBlueprintNode)
   : m_blueprint(nullptr)
{
   PopulateFromXMLNode(itemBlueprintNode);
}


//-----------------------------------------------------------------------------------------------
ItemFactory::~ItemFactory()
{
   if (m_blueprint != nullptr)
   {
      delete m_blueprint;
      m_blueprint = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
Item* ItemFactory::CreateItem() const 
{
   Item* newItem = new Item(*m_blueprint);
   return newItem;
}


//-----------------------------------------------------------------------------------------------
Item* ItemFactory::CreateFromNode(const XMLNode& node) const
{
   Item* newFeature = new Item(*m_blueprint, node);
   return newFeature;
}


//-----------------------------------------------------------------------------------------------
STATIC void ItemFactory::LoadAllItemBlueprints()
{
   std::vector<std::string> itemFiles = EnumerateFilesInDirectory("Data/Items", "*.Item.xml");

   // If one is null, they all are
   if (itemFiles.size() > 0 && s_factoryMaps[0] == nullptr)
   {
      for (size_t factoryIndex = 0; factoryIndex < NUM_ITEM_TYPES; ++factoryIndex)
      {
         s_factoryMaps[factoryIndex] = new ItemFactoryMap;
      }
   }

   for (const std::string& itemFile : itemFiles)
   {
      XMLNode itemFileNode = XMLNode::openFileHelper(itemFile.c_str(), "ItemBlueprints");

      int itemIndex = 0;
      XMLNode itemNode = itemFileNode.getChildNode("ItemBlueprint", &itemIndex);
      while (!itemNode.isEmpty())
      {
         ItemFactory* newFactory = new ItemFactory(itemNode);

         std::string itemName = newFactory->m_blueprint->GetName();
         ItemType newItemType = newFactory->m_blueprint->GetItemType();

         s_factoryMaps[newItemType]->insert(ItemFactoryPair(itemName, newFactory));

         itemNode = itemFileNode.getChildNode("ItemBlueprint", &itemIndex);
      }
   }
}


//-----------------------------------------------------------------------------------------------
STATIC void ItemFactory::FreeAllItemBlueprints()
{
   for (size_t mapIndex = 0; mapIndex < NUM_ITEM_TYPES; ++mapIndex)
   {
      ItemFactoryMap* currentMap = s_factoryMaps[mapIndex];
      ItemFactoryIter mapIter = currentMap->begin();
      while (mapIter != currentMap->end())
      {
         delete mapIter->second;
         mapIter->second = nullptr;
         ++mapIter;
      }

      delete currentMap;
      s_factoryMaps[mapIndex] = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
STATIC Item* ItemFactory::CreateItemFromXMLNode(const XMLNode& node)
{
   std::string itemName = node.getAttribute("name");

   for (int mapIndex = 0; mapIndex < NUM_ITEM_TYPES; ++mapIndex)
   {
      ItemFactoryMap* factoryMap = s_factoryMaps[mapIndex];
      ItemFactoryIter factoryIter = factoryMap->find(itemName);

      if (factoryIter != factoryMap->end())
      {
         ItemFactory* targetFactory = factoryIter->second;
         return targetFactory->CreateFromNode(node);
      }
   }

   return nullptr;
}


//-----------------------------------------------------------------------------------------------
STATIC Item* ItemFactory::CreateByNameOfType(const std::string& name, ItemType type)
{
   ItemFactoryMap* factoryMap = s_factoryMaps[type];

   ItemFactoryIter factoryIter = factoryMap->find(name);
   ASSERT_OR_DIE(factoryIter != factoryMap->end(), Stringf("ERROR: No factory found for Item %s", name.c_str()));

   ItemFactory* targetFactory = factoryIter->second;
   return targetFactory->CreateItem();
}


//-----------------------------------------------------------------------------------------------
STATIC Item* ItemFactory::CreateRandomItem()
{
   IntRange range(0, NUM_ITEM_TYPES - 1);
   ItemType type = (ItemType)GetRandomIntBetweenInclusive(range);
   return CreateRandomItemOfType(type);
}


//-----------------------------------------------------------------------------------------------
STATIC Item* ItemFactory::CreateRandomItemOfType(ItemType type)
{
   ItemFactoryMap* factoryMap = s_factoryMaps[type];

   IntRange range(0, factoryMap->size() - 1);
   ItemFactoryIter factoryIter = factoryMap->begin();
   std::advance(factoryIter, GetRandomIntBetweenInclusive(range));

   return factoryIter->second->CreateItem();
}


//-----------------------------------------------------------------------------------------------
void ItemFactory::PopulateFromXMLNode(const XMLNode& itemBlueprintNode)
{
   m_blueprint = new Item(itemBlueprintNode);
}