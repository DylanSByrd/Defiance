#pragma once

#include <string>
#include <map>


//-----------------------------------------------------------------------------------------------
class NPC;
struct XMLNode;
class NPCFactory;
typedef std::map<std::string, NPCFactory*> NPCFactoryMap;


//-----------------------------------------------------------------------------------------------
class NPCFactory
{
public:
   NPCFactory(const XMLNode& npcBlueprintNode);
   ~NPCFactory();

   NPC* CreateNPC() const;
   NPC* CreateFromNode(const XMLNode& node) const;

   static void LoadAllNPCBlueprints();
   static void FreeAllNPCBlueprints();
   static NPC* CreateNPCFromXMLNode(const XMLNode& node);
   static NPC* CreateByName(const std::string& name);
   static NPCFactoryMap* s_factoryMap;

private:
   void PopulateFromXMLNode(const XMLNode& npcBlueprintNode);

   NPC* m_blueprint;
   int m_minHealth;
   int m_maxHealth;
};
typedef std::pair<std::string, NPCFactory*> NPCFactoryPair;