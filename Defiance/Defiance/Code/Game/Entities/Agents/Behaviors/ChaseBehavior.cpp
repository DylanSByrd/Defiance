#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Pathfinding/PathfinderAStar.hpp"
#include "Game/Entities/Agents/Behaviors/ChaseBehavior.hpp"
#include "Game/Entities/Agents/Agent.hpp"


//-----------------------------------------------------------------------------------------------
STATIC BehaviorRegistration ChaseBehavior::s_chaseBehaviorRegistration("Chase", &ChaseBehavior::CreateBehavior);


//-----------------------------------------------------------------------------------------------
ChaseBehavior::ChaseBehavior(const std::string& name, const XMLNode& blueprintNode)
   : Behavior(name, blueprintNode)
   , m_tilesFromStartToChase(15)
   , m_turnsToChase(15)
   , m_chaseTarget(nullptr)
{
   PopulateFromXMLNode(blueprintNode);
}


//-----------------------------------------------------------------------------------------------
ChaseBehavior::ChaseBehavior(const ChaseBehavior& copySource)
   : Behavior(copySource)
   , m_tilesFromStartToChase(copySource.m_tilesFromStartToChase)
   , m_turnsToChase(copySource.m_turnsToChase)
   , m_chaseTarget(copySource.m_chaseTarget)
{}


//-----------------------------------------------------------------------------------------------
float ChaseBehavior::CalcUtility()
{
   m_chaseTarget = m_owningAgent->GetClosestEnemy();
   if (m_chaseTarget == nullptr)
   {
      return 0.f;
   }

   TileCoords agentPos = m_owningAgent->GetPosition();
   int tilesFromTarget = agentPos.GetManhattanDistanceToVector(m_chaseTarget->GetPosition());
   if (tilesFromTarget <= 1)
   {
      return 0.f;
   }

   return 3.f;
}


//-----------------------------------------------------------------------------------------------
void ChaseBehavior::Run()
{
   TileCoords myAgentPos = m_owningAgent->GetPosition();
   TileCoords targetAgentPos = m_chaseTarget->GetPosition();

   Map* gameMap = m_owningAgent->GetMap();
   PathfinderAStar pathfinder(myAgentPos, targetAgentPos, MapProxy(gameMap));
   pathfinder.FindPath();

   PathfinderResult result = pathfinder.GetResult();

   if (result == NO_PATH)
   {
      return;
   }

   const Path& path = pathfinder.GetPath();
   TileDirection directionToTarget = gameMap->GetDirectionFromSourceToDest(myAgentPos, path.GetStepTowardsPathEnd());
   m_owningAgent->MoveOneStepInDirection(directionToTarget);
}


//-----------------------------------------------------------------------------------------------
Behavior* ChaseBehavior::Clone()
{
   Behavior* clone = new ChaseBehavior(*this);
   return clone;
}


//-----------------------------------------------------------------------------------------------
void ChaseBehavior::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
   m_tilesFromStartToChase = ReadXMLAttribute(blueprintNode, "tilesFromStartToChase", m_tilesFromStartToChase);
   m_turnsToChase = ReadXMLAttribute(blueprintNode, "turnsToChase", m_turnsToChase);
}


//-----------------------------------------------------------------------------------------------
bool ChaseBehavior::DoesPassChanceToRun()
{
   // #TODO
   return true;
}


//-----------------------------------------------------------------------------------------------
void ChaseBehavior::DereferenceEntity(Entity* entity)
{
   if (m_chaseTarget == entity)
   {
      m_chaseTarget = nullptr;
   }
}