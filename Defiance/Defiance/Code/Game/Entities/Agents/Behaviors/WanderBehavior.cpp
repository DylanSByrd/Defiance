#include "Engine/Parsers/XMLUtilities.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Entities/Agents/Behaviors/WanderBehavior.hpp"
#include "Game/Entities/Agents/NPCs/NPC.hpp"


//-----------------------------------------------------------------------------------------------
STATIC BehaviorRegistration WanderBehavior::s_wanderBehaviorRegistrastion("Wander", &WanderBehavior::CreateBehavior);


//-----------------------------------------------------------------------------------------------
WanderBehavior::WanderBehavior(const std::string& name, const XMLNode& blueprintNode)
   : Behavior(name, blueprintNode)
   , m_chanceToRest(0.5f)
   , m_chanceToGoStraight(0.3f)
   , m_lastDirection(INVALID_DIRECTION)
{
   PopulateFromXMLNode(blueprintNode);
}


//-----------------------------------------------------------------------------------------------
WanderBehavior::WanderBehavior(const WanderBehavior& copySource)
   : Behavior(copySource)
   , m_chanceToRest(copySource.m_chanceToRest)
   , m_chanceToGoStraight(copySource.m_chanceToGoStraight)
   , m_lastDirection(copySource.m_lastDirection)
{}


//-----------------------------------------------------------------------------------------------
float WanderBehavior::CalcUtility()
{
   // #TODO
   return 1.f;
}


//-----------------------------------------------------------------------------------------------
void WanderBehavior::Run()
{
   bool isResting = GetRandomTrueOrFalseWithProbability(m_chanceToRest);
   if (isResting)
   {
      m_lastDirection = INVALID_DIRECTION;
      return;
   }

   if (m_lastDirection != INVALID_DIRECTION)
   {
      bool isGoingStraight = GetRandomTrueOrFalseWithProbability(m_chanceToGoStraight);
      if (isGoingStraight)
      {
         bool didMove = m_owningAgent->MoveOneStepInDirection(m_lastDirection);
         if (didMove)
         {
            return;
         }
      }
   }

   TileDirection newDirection = GetRandomTileDirection();
   while (newDirection == m_lastDirection)
   {
      newDirection = GetRandomTileDirection();
   }

   // If stuck, we'll just rest I guess
   bool didMove = m_owningAgent->MoveOneStepInDirection(newDirection);
   if (didMove)
   {
      m_lastDirection = newDirection;
   }
   else
   {
      m_lastDirection = INVALID_DIRECTION;
   }
}


//-----------------------------------------------------------------------------------------------
Behavior* WanderBehavior::Clone()
{
   Behavior* clone = new WanderBehavior(*this);
   return clone;
}


//-----------------------------------------------------------------------------------------------
void WanderBehavior::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
   m_chanceToRest = ReadXMLAttribute(blueprintNode, "chanceToRest", m_chanceToRest);
   m_chanceToGoStraight = ReadXMLAttribute(blueprintNode, "chanceToGoStraight", m_chanceToGoStraight);
}


//-----------------------------------------------------------------------------------------------
bool WanderBehavior::DoesPassChanceToRun()
{
   // #TODO
   return true;
}