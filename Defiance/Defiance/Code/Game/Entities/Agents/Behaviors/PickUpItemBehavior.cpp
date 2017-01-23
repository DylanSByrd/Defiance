#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Pathfinding/PathfinderAStar.hpp"
#include "Game/Entities/Agents/Behaviors/PickUpItemBehavior.hpp"
#include "Game/Entities/Agents/Agent.hpp"


//-----------------------------------------------------------------------------------------------
STATIC BehaviorRegistration PickUpItemBehavior::s_pickUpItemBehaviorRegistration("PickUpItem", &PickUpItemBehavior::CreateBehavior);


//-----------------------------------------------------------------------------------------------
PickUpItemBehavior::PickUpItemBehavior(const std::string& name, const XMLNode& blueprintNode)
   : Behavior(name, blueprintNode)
   , m_tilesToTravelForItem(15)
   , m_targetItem(nullptr)
{
   PopulateFromXMLNode(blueprintNode);
}


//-----------------------------------------------------------------------------------------------
PickUpItemBehavior::PickUpItemBehavior(const PickUpItemBehavior& copySource)
   : Behavior(copySource)
   , m_tilesToTravelForItem(copySource.m_tilesToTravelForItem)
   , m_targetItem(copySource.m_targetItem)
{
}


//-----------------------------------------------------------------------------------------------
float PickUpItemBehavior::CalcUtility()
{
   if (m_owningAgent->IsInventoryFull())
   {
      return 0.f;
   }

   m_targetItem = m_owningAgent->GetClosestVisibleItem();
   if (m_targetItem == nullptr)
   {
      return 0.f;
   }

   TileCoords agentPos = m_owningAgent->GetPosition();
   int tilesFromTarget = agentPos.GetManhattanDistanceToVector(m_targetItem->GetPosition());
   if (tilesFromTarget > m_tilesToTravelForItem)
   {
      return 0.f;
   }

   return 2.f;
}


//-----------------------------------------------------------------------------------------------
void PickUpItemBehavior::Run()
{
   // sanity check
   if (m_targetItem == nullptr)
   {
      return;
   }

   TileCoords agentPos = m_owningAgent->GetPosition();
   TileCoords itemPos = m_targetItem->GetPosition();
   int tilesFromTarget = agentPos.GetManhattanDistanceToVector(itemPos);
   
   // We're standing on it; pick it up!
   if (tilesFromTarget == 0)
   {
      m_owningAgent->TryGetItemFromCurrentTile();
      m_targetItem = nullptr;
      return;
   }


   Map* gameMap = m_owningAgent->GetMap();
   PathfinderAStar pathfinder(agentPos, itemPos, MapProxy(gameMap));
   pathfinder.FindPath();

   PathfinderResult result = pathfinder.GetResult();
   if (result == NO_PATH)
   {
      return;
   }

   const Path& path = pathfinder.GetPath();
   TileDirection directionToTarget = gameMap->GetDirectionFromSourceToDest(agentPos, path.GetStepTowardsPathEnd());
   m_owningAgent->MoveOneStepInDirection(directionToTarget);
}


//-----------------------------------------------------------------------------------------------
Behavior* PickUpItemBehavior::Clone()
{
   Behavior* clone = new PickUpItemBehavior(*this);
   return clone;
}


//-----------------------------------------------------------------------------------------------
void PickUpItemBehavior::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
   m_tilesToTravelForItem = ReadXMLAttribute(blueprintNode, "tilesToTravelForItem", m_tilesToTravelForItem);
}


//-----------------------------------------------------------------------------------------------
bool PickUpItemBehavior::DoesPassChanceToRun()
{
   // #TODO
   return true;
}


//-----------------------------------------------------------------------------------------------
void PickUpItemBehavior::DereferenceEntity(Entity* entity)
{
   if (m_targetItem == entity)
   {
      m_targetItem = nullptr;
   }
}


