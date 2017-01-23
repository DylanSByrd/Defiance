#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Core/GameContext.hpp"
#include "Game/IO/SaveGame.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Entities/Agents/Player.hpp"

//-----------------------------------------------------------------------------------------------
STATIC const char* SaveGame::SAVE_FILE_PATH = "Data/Saves/SaveGame.Save.xml";


//-----------------------------------------------------------------------------------------------
SaveGame::SaveGame(GameContext* context)
   : m_context(context)
{}


//-----------------------------------------------------------------------------------------------
bool SaveGame::Save()
{
   bool saveSuccessful = true;

   XMLNode saveNode = XMLNode::createXMLTopNode("SaveData");
   
   if (!SaveMapData(saveNode))
   {
      return !saveSuccessful;
   }

   XMLNode gameStatsNode = saveNode.addChild("GameStats");
   gameStatsNode.addAttribute("monstersKilled", std::to_string(m_context->numberOfEnemiesSlainByPlayer).c_str());
   gameStatsNode.addAttribute("stepsTaken", std::to_string(m_context->numberOfPlayerStepsTaken).c_str());

   if (!SaveEntities(saveNode))
   {
      return !saveSuccessful;
   }


   // #TODO - error check here
   saveNode.writeToFile(SAVE_FILE_PATH);
   return saveSuccessful;
}


//-----------------------------------------------------------------------------------------------
bool SaveGame::SaveMapData(XMLNode& topNode)
{
   Map* activeMap = m_context->activeMap;
   return activeMap->WriteToXMLNode(topNode);
}


//-----------------------------------------------------------------------------------------------
bool SaveGame::SaveEntities(XMLNode& topNode)
{
   bool saveSuccessful = true;

   XMLNode entityData = topNode.addChild("EntityData");

   // For testing, start with just player
   for (Entity* entity : m_context->activeEntities)
   {
      if (!entity->WriteToXMLNode(entityData))
      {
         return !saveSuccessful;
      }
   }

   return saveSuccessful;
}