#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Tokenizer.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Agents/Behaviors/HealAllyBehavior.hpp"
#include "Game/Combat/DefianceCombatSystem.hpp"
#include "Game/Entities/Agents/Agent.hpp"


//-----------------------------------------------------------------------------------------------
STATIC BehaviorRegistration HealAllyBehavior::s_healAllyBehaviorRegistration("HealAlly", &HealAllyBehavior::CreateBehavior);


//-----------------------------------------------------------------------------------------------
HealAllyBehavior::HealAllyBehavior(const std::string& name, const XMLNode& blueprintNode)
   : Behavior(name, blueprintNode)
   , m_minHealthToHeal(1)
   , m_maxHealthToHeal(3)
   , m_percentChanceToHit(.8f)
   , m_healTarget(nullptr)
{
   PopulateFromXMLNode(blueprintNode);
}


//-----------------------------------------------------------------------------------------------
HealAllyBehavior::HealAllyBehavior(const HealAllyBehavior& copySource)
   : Behavior(copySource)
   , m_minHealthToHeal(copySource.m_minHealthToHeal)
   , m_maxHealthToHeal(copySource.m_maxHealthToHeal)
   , m_percentChanceToHit(copySource.m_percentChanceToHit)
   , m_healTarget(copySource.m_healTarget)
{
}


//-----------------------------------------------------------------------------------------------
float HealAllyBehavior::CalcUtility()
{
   m_healTarget = m_owningAgent->GetClosestAlly();
   if (m_healTarget == nullptr
      || m_healTarget->IsAtMaxHealth())
   {
      return 0.f;
   }

   float distanceToTarget = Vector2i::GetDistanceBetween(m_owningAgent->GetPosition(), m_healTarget->GetPosition());
   if (distanceToTarget >= 2.f) // 1.4f == diagonal
   {
      return 0.f;
   }

   return 6.f;
}


//-----------------------------------------------------------------------------------------------
void HealAllyBehavior::Run()
{
   // sanity check
   if (m_healTarget == nullptr)
   {
      return;
   }

   AttackData attackData;
   attackData.minDamage = -m_minHealthToHeal;
   attackData.maxDamage = -m_maxHealthToHeal;
   attackData.percentChanceToHit = m_percentChanceToHit;
   attackData.instigator = m_owningAgent;
   attackData.target = m_healTarget;

   m_owningAgent->PerformAttack(attackData);
}


//-----------------------------------------------------------------------------------------------
Behavior* HealAllyBehavior::Clone()
{
   Behavior* clone = new HealAllyBehavior(*this);
   return clone;
}


//-----------------------------------------------------------------------------------------------
void HealAllyBehavior::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
   Tokenizer tokenizer(ReadXMLAttribute(blueprintNode, "healingPower", std::string("1~3")), "~");
   SetTypeFromString(m_minHealthToHeal, tokenizer.GetNextToken());

   if (tokenizer.IsFinished())
   {
      m_maxHealthToHeal = m_minHealthToHeal;
   }
   else
   {
      SetTypeFromString(m_maxHealthToHeal, tokenizer.GetNextToken());
   }

   m_percentChanceToHit = ReadXMLAttribute(blueprintNode, "percentChanceToHit", m_percentChanceToHit);
}


//-----------------------------------------------------------------------------------------------
bool HealAllyBehavior::DoesPassChanceToRun()
{
   //#TODO
   return true;
}


//-----------------------------------------------------------------------------------------------
void HealAllyBehavior::DereferenceEntity(Entity* entity)
{
   if (m_healTarget == entity)
   {
      m_healTarget = nullptr;
   }
}