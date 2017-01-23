#pragma once

#include <map>


//-----------------------------------------------------------------------------------------------
struct XMLNode;
struct GameContext;


//-----------------------------------------------------------------------------------------------
class LoadGame
{
public:
   LoadGame(GameContext* context);
   bool Load();
   bool LoadMapData(XMLNode& topNode);
   bool LoadEntities(XMLNode& topNode);
   void ResolveEntityPointers(std::map<int, Entity*>& loadedEntities);

   static const char* LOAD_FILE_PATH;

private:
   GameContext* m_context;
};