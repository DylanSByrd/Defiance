#include <vector>
#include <string>
#include "Engine/IO/FileUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Tokenizer.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Entities/Agents/NPCs/NPCFactory.hpp"
#include "Game/Entities/Agents/NPCs/NPC.hpp"


//-----------------------------------------------------------------------------------------------
STATIC NPCFactoryMap* NPCFactory::s_factoryMap = nullptr;


//-----------------------------------------------------------------------------------------------
NPCFactory::NPCFactory(const XMLNode& npcBlueprintNode)
   : m_blueprint(nullptr)
   , m_minHealth(10)
   , m_maxHealth(10)
{
   PopulateFromXMLNode(npcBlueprintNode);
}


//-----------------------------------------------------------------------------------------------
NPCFactory::~NPCFactory()
{
   if (m_blueprint != nullptr)
   {
      delete m_blueprint;
      m_blueprint = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
NPC* NPCFactory::CreateNPC() const
{
   NPC* newNPC = new NPC(*m_blueprint);
   
   int npcHealth = GetRandomIntBetweenInclusive(m_minHealth, m_maxHealth);
   
   newNPC->SetMaxHealth(npcHealth);
   newNPC->SetCurrentHealth(npcHealth);

   return newNPC;
}


//-----------------------------------------------------------------------------------------------
NPC* NPCFactory::CreateFromNode(const XMLNode& node) const
{
   NPC* newNPC = new NPC(*m_blueprint, node);
   return newNPC;
}


//-----------------------------------------------------------------------------------------------
STATIC void NPCFactory::LoadAllNPCBlueprints()
{
   std::vector<std::string> npcFiles = EnumerateFilesInDirectory("Data/NPCs", "*.NPC.xml");

   if (npcFiles.size() > 0 && s_factoryMap == nullptr)
   {
      s_factoryMap = new NPCFactoryMap;
   }

   for (std::string& npcFile : npcFiles)
   {
      // I thought we said NPCs in class, but Prof's XML had this instead
      XMLNode npcFileNode = XMLNode::openFileHelper(npcFile.c_str(), "NPCBlueprints");
      
      int npcIndex = 0;
      XMLNode npcNode = npcFileNode.getChildNode("NPCBlueprint", &npcIndex);
      while (!npcNode.isEmpty())
      {
         NPCFactory* newFactory = new NPCFactory(npcNode);
         s_factoryMap->insert(NPCFactoryPair(newFactory->m_blueprint->GetName(), newFactory));
         npcNode = npcFileNode.getChildNode("NPCBlueprint", &npcIndex);
      }
   }
}


//-----------------------------------------------------------------------------------------------
STATIC void NPCFactory::FreeAllNPCBlueprints()
{
   NPCFactoryMap::iterator factoryIter = s_factoryMap->begin();
   while (factoryIter != s_factoryMap->end())
   {
      delete factoryIter->second;
      factoryIter->second = nullptr;
      ++factoryIter;
   }

   delete s_factoryMap;
   s_factoryMap = nullptr;
}


//-----------------------------------------------------------------------------------------------
STATIC NPC* NPCFactory::CreateNPCFromXMLNode(const XMLNode& node)
{
   NPCFactoryMap::iterator factoryIter = s_factoryMap->find(node.getAttribute("name"));
   if (factoryIter != s_factoryMap->end())
   {
      NPCFactory* targetFactory = factoryIter->second;
      return targetFactory->CreateFromNode(node);
   }

   return nullptr;
}


//-----------------------------------------------------------------------------------------------
STATIC NPC* NPCFactory::CreateByName(const std::string& name)
{
   NPCFactoryMap::iterator factoryIter = s_factoryMap->find(name);
   ASSERT_OR_DIE(factoryIter != s_factoryMap->end(), Stringf("ERROR: No factory found for NPC %s", name.c_str()));
   
   NPCFactory* targetFactory = factoryIter->second;
   return targetFactory->CreateNPC();
}


//-----------------------------------------------------------------------------------------------
void NPCFactory::PopulateFromXMLNode(const XMLNode& npcBlueprintNode)
{
   m_blueprint = new NPC(npcBlueprintNode);

   Tokenizer tokenizer(ReadXMLAttribute(npcBlueprintNode, "maxHealth", std::string("10")), "~");
   SetTypeFromString(m_minHealth, tokenizer.GetNextToken());

   if (tokenizer.IsFinished())
   {
      m_maxHealth = m_minHealth;
   }
   else
   {
      SetTypeFromString(m_maxHealth, tokenizer.GetNextToken());
   }
}


