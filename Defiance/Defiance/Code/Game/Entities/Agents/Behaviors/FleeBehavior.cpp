#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Agents/Behaviors/FleeBehavior.hpp"
#include "Game/Entities/Agents/Agent.hpp"
#include "Game/Map/Map.hpp"
#include "Game/UI/GameMessageBox.hpp"


//-----------------------------------------------------------------------------------------------
STATIC BehaviorRegistration FleeBehavior::s_fleeBehaviorRegistration("Flee", &FleeBehavior::CreateBehavior);


//-----------------------------------------------------------------------------------------------
FleeBehavior::FleeBehavior(const std::string& name, const XMLNode& blueprintNode)
   : Behavior(name, blueprintNode)
   , m_healthThreshold(3)
   , m_isFleeing(false)
{
   PopulateFromXMLNode(blueprintNode);
}


//-----------------------------------------------------------------------------------------------
FleeBehavior::FleeBehavior(const FleeBehavior& copySource)
   : Behavior(copySource)
   , m_healthThreshold(copySource.m_healthThreshold)
   , m_isFleeing(copySource.m_isFleeing)
{}


//-----------------------------------------------------------------------------------------------
float FleeBehavior::CalcUtility()
{
   if (m_owningAgent->GetCurrentHealth() > m_healthThreshold)
   {
      m_isFleeing = false;
      return 0.f;
   }

   Agent* closestThreat = m_owningAgent->GetClosestEnemy();
   if (closestThreat == nullptr
      || Vector2i::GetDistanceBetween(m_owningAgent->GetPosition(), closestThreat->GetPosition()) > 5.f)
   {
      m_isFleeing = false;
      return 0.f;
   }

   return 10.f;
}


//-----------------------------------------------------------------------------------------------
void FleeBehavior::Run()
{
   TileCoords pos = m_owningAgent->GetPosition();
   TileCoords threatPos = m_owningAgent->GetClosestEnemy()->GetPosition();

   TileDirection dirToThreat = m_owningAgent->GetMap()->GetDirectionFromSourceToDest(pos, threatPos);
   TileDirection dirAwayFromThreat = Map::GetOppositeTileDirection(dirToThreat);

   m_owningAgent->MoveOneStepInDirection(dirAwayFromThreat);

   if (!m_isFleeing)
   {
      m_isFleeing = true;
      g_theGameMessageBox->PrintNeutralMessage(Stringf("The %s flees!", m_owningAgent->GetName().c_str()));
   }
}


//-----------------------------------------------------------------------------------------------
Behavior* FleeBehavior::Clone()
{
   Behavior* clone = new FleeBehavior(*this);
   return clone;
}


//-----------------------------------------------------------------------------------------------
void FleeBehavior::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
   m_healthThreshold = ReadXMLAttribute(blueprintNode, "healthThreshold", m_healthThreshold);
}


//-----------------------------------------------------------------------------------------------
bool FleeBehavior::DoesPassChanceToRun()
{
   // #TODO
   return true;
}