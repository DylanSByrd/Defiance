#pragma once

#include <string>


//-----------------------------------------------------------------------------------------------
struct XMLNode;
struct GameContext;


//-----------------------------------------------------------------------------------------------
class SaveGame
{
public:
   SaveGame(GameContext* context);
   bool Save();
   bool SaveMapData(XMLNode& topNode);
   bool SaveEntities(XMLNode& topNode);

   static const char* SAVE_FILE_PATH;

private:
   GameContext* m_context;
};