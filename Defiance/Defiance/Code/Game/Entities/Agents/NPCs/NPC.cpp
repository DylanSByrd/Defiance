#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Agents/NPCs/NPC.hpp"
#include "Game/Entities/Agents/Behaviors/Behavior.hpp"


//-----------------------------------------------------------------------------------------------
NPC::NPC(const XMLNode& blueprintNode)
   : Agent(blueprintNode)
{
   PopulateFromXMLNode(blueprintNode);
}


//-----------------------------------------------------------------------------------------------
NPC::NPC(const NPC& copySource)
   : Agent(copySource)
{
}


//-----------------------------------------------------------------------------------------------
NPC::NPC(const NPC& copySource, const XMLNode& node)
   : Agent(copySource, node)
{
}


//-----------------------------------------------------------------------------------------------
void NPC::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
   XMLNode behaviorRootNode = blueprintNode.getChildNode("Behaviors");
   PopulateBehaviorsFromXMLNode(behaviorRootNode);
}


//-----------------------------------------------------------------------------------------------
void NPC::PopulateBehaviorsFromXMLNode(const XMLNode& behaviorRootNode)
{
   if (behaviorRootNode.IsContentEmpty())
   {
      return;
   }

   int behaviorIndex = 0;
   XMLNode behaviorNode = behaviorRootNode.getChildNode(behaviorIndex);
   while (!behaviorNode.isEmpty())
   {
      std::string behaviorName = behaviorNode.getName();
      Behavior* newBehavior = BehaviorRegistration::CreateBehaviorByName(behaviorName, behaviorNode);

      m_behaviors.push_back(newBehavior);
      ++behaviorIndex;
      behaviorNode = behaviorRootNode.getChildNode(behaviorIndex);
   }
}


//-----------------------------------------------------------------------------------------------
bool NPC::WriteToXMLNode(XMLNode& entityNode) const
{
   bool writeSuccessful = true;

   XMLNode npcNode = entityNode.addChild("NPCBlueprint");
   if (!Agent::WriteToXMLNode(npcNode))
   {
      return !writeSuccessful;
   }

   return writeSuccessful;
}

