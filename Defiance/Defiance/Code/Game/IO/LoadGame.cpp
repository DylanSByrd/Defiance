#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Core/GameContext.hpp"
#include "Game/IO/LoadGame.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Entities/Agents/Player.hpp"
#include "Game/Entities/Agents/NPCs/NPC.hpp"
#include "Game/Entities/Agents/NPCs/NPCFactory.hpp"
#include "Game/Entities/Items/ItemFactory.hpp"
#include "Game/Entities/Features/FeatureFactory.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const char* LoadGame::LOAD_FILE_PATH = "Data/Saves/SaveGame.Save.xml";


//-----------------------------------------------------------------------------------------------
LoadGame::LoadGame(GameContext* context)
   : m_context(context)
{}


//-----------------------------------------------------------------------------------------------
bool LoadGame::Load()
{
   bool loadSuccessful = true;

   if (m_context == nullptr)
   {
      m_context = new GameContext;
   }

   XMLNode loadNode = XMLNode::openFileHelper(LOAD_FILE_PATH, "SaveData");

   if (loadNode.isEmpty())
   {
      return !loadSuccessful;
   }

   if (!LoadMapData(loadNode))
   {
      return !loadSuccessful;
   }

   XMLNode statsNode = loadNode.getChildNode("GameStats");
   m_context->numberOfEnemiesSlainByPlayer = ReadXMLAttribute(statsNode, "monstersKilled", 0);
   m_context->numberOfPlayerStepsTaken = ReadXMLAttribute(statsNode, "stepsTaken", 0);

   if (!LoadEntities(loadNode))
   {
      return !loadSuccessful;
   }

   remove(LOAD_FILE_PATH);
   return loadSuccessful;
}


//-----------------------------------------------------------------------------------------------
bool LoadGame::LoadMapData(XMLNode& topNode)
{
   bool mapLoadSuccessful = true;

   Map*& activeMap = m_context->activeMap;
   activeMap = new Map();

   XMLNode mapNode = topNode.getChildNode("MapData");
   if (!activeMap->InitToXMLNode(mapNode, "Loaded Map"))
   {
      return !mapLoadSuccessful;
   }

   return mapLoadSuccessful;
}


//-----------------------------------------------------------------------------------------------
bool LoadGame::LoadEntities(XMLNode& topNode)
{
   bool loadSuccessful = true;

   Map* activeMap = m_context->activeMap;

   std::map<int, Entity*> loadedEntities;
   XMLNode entityData = topNode.getChildNode("EntityData");

   // For testing, start with just player
   int entityIndex = 0;
   XMLNode entityNode = entityData.getChildNode(entityIndex);
   while (!entityNode.isEmpty())
   {
      int savedID = ReadXMLAttribute(entityNode, "EntityID", 0);
      
      std::string nodeName = entityNode.getName();
      if (!strcmp(nodeName.c_str(), "PlayerBlueprint"))
      {
         Player* newPlayer = new Player(entityNode);

         loadedEntities.insert(std::pair<int, Entity*>(savedID, newPlayer));
         m_context->activePlayer = newPlayer;
         m_context->activeAgents.insert(TurnOrderMapPair(0.f, newPlayer));
         m_context->activeEntities.push_back(newPlayer);
         newPlayer->AddToMap(activeMap, newPlayer->GetPosition());
      }
      else if (!strcmp(nodeName.c_str(), "NPCBlueprint"))
      {
         NPC* newNPC = NPCFactory::CreateNPCFromXMLNode(entityNode);

         loadedEntities.insert(std::pair<int, Entity*>(savedID, newNPC));
         m_context->activeAgents.insert(TurnOrderMapPair(1.f, newNPC));
         m_context->activeEntities.push_back(newNPC);
         newNPC->AddToMap(activeMap, newNPC->GetPosition());
      }
      else if (!strcmp(nodeName.c_str(), "FeatureBlueprint"))
      {
         Feature* newFeature = FeatureFactory::CreateFeatureFromXMLNode(entityNode);

         loadedEntities.insert(std::pair<int, Entity*>(savedID, newFeature));
         m_context->activeEntities.push_back(newFeature);
         newFeature->AddToMap(activeMap, newFeature->GetPosition());
      }
      else if (!strcmp(nodeName.c_str(), "ItemBlueprint"))
      {
         Item* newItem = ItemFactory::CreateItemFromXMLNode(entityNode);

         loadedEntities.insert(std::pair<int, Entity*>(savedID, newItem));
         m_context->activeEntities.push_back(newItem);
         if (!newItem->IsPickedUp())
         {
            newItem->AddToMap(activeMap, newItem->GetPosition());
         }
      }

      ++entityIndex;
      entityNode = entityData.getChildNode(entityIndex);
   }

   ResolveEntityPointers(loadedEntities);

   return loadSuccessful;
}


//-----------------------------------------------------------------------------------------------
void LoadGame::ResolveEntityPointers(std::map<int, Entity*>& loadedEntities)
{
   for (std::map<int, Entity*>::iterator entityIter = loadedEntities.begin(); entityIter != loadedEntities.end(); ++entityIter)
   {
      if (entityIter->second->IsAgent())
      {
         entityIter->second->ResolveEntityPointers(loadedEntities);
      }
   }
}


