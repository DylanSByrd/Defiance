#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Tokenizer.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Agents/Behaviors/MeleeAttackBehavior.hpp"
#include "Game/Combat/DefianceCombatSystem.hpp"
#include "Game/Entities/Agents/Agent.hpp"


//-----------------------------------------------------------------------------------------------
STATIC BehaviorRegistration MeleeAttackBehavior::s_meleeAttackBehaviorRegistration("MeleeAttack", &MeleeAttackBehavior::CreateBehavior);


//-----------------------------------------------------------------------------------------------
MeleeAttackBehavior::MeleeAttackBehavior(const std::string& name, const XMLNode& blueprintNode)
   : Behavior(name, blueprintNode)
   , m_minDamage(1)
   , m_maxDamage(3)
   , m_percentChanceToHit(.8f)
   , m_meleeAttackTarget(nullptr)
{
   PopulateFromXMLNode(blueprintNode);
}


//-----------------------------------------------------------------------------------------------
MeleeAttackBehavior::MeleeAttackBehavior(const MeleeAttackBehavior& copySource)
   : Behavior(copySource)
   , m_minDamage(copySource.m_minDamage)
   , m_maxDamage(copySource.m_maxDamage)
   , m_percentChanceToHit(copySource.m_percentChanceToHit)
   , m_meleeAttackTarget(copySource.m_meleeAttackTarget)
{
}


//-----------------------------------------------------------------------------------------------
float MeleeAttackBehavior::CalcUtility()
{
   m_meleeAttackTarget = m_owningAgent->GetClosestEnemy();
   if (m_meleeAttackTarget == nullptr)
   {
      return 0.f;
   }

   float distanceToTarget = Vector2i::GetDistanceBetween(m_owningAgent->GetPosition(), m_meleeAttackTarget->GetPosition());
   if (distanceToTarget >= 2.f) // 1.4f == diagonal
   {
      return 0.f;
   }

   return 5.f;
}


//-----------------------------------------------------------------------------------------------
void MeleeAttackBehavior::Run()
{
   // sanity check
   if (m_meleeAttackTarget == nullptr)
   {
      return;
   }

   AttackData attackData;
   attackData.minDamage = m_minDamage;
   attackData.maxDamage = m_maxDamage;
   attackData.percentChanceToHit = m_percentChanceToHit;
   attackData.instigator = m_owningAgent;
   attackData.target = m_meleeAttackTarget;

   m_owningAgent->PerformAttack(attackData);
}


//-----------------------------------------------------------------------------------------------
Behavior* MeleeAttackBehavior::Clone()
{
   Behavior* clone = new MeleeAttackBehavior(*this);
   return clone;
}


//-----------------------------------------------------------------------------------------------
void MeleeAttackBehavior::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
   Tokenizer tokenizer(ReadXMLAttribute(blueprintNode, "damage", std::string("1~3")), "~");
   SetTypeFromString(m_minDamage, tokenizer.GetNextToken());

   if (tokenizer.IsFinished())
   {
      m_maxDamage = m_minDamage;
   }
   else
   {
      SetTypeFromString(m_maxDamage, tokenizer.GetNextToken());
   }

   m_percentChanceToHit = ReadXMLAttribute(blueprintNode, "percentChanceToHit", 0.8f);
}


//-----------------------------------------------------------------------------------------------
bool MeleeAttackBehavior::DoesPassChanceToRun()
{
   //#TODO
   return true;
}


//-----------------------------------------------------------------------------------------------
void MeleeAttackBehavior::DereferenceEntity(Entity* entity)
{
   if (m_meleeAttackTarget == entity)
   {
      m_meleeAttackTarget = nullptr;
   }
}