#pragma once

#include <string>
#include "Game/Entities/Agents/Behaviors/Behavior.hpp"
#include "Game/Core/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class Entity;


//-----------------------------------------------------------------------------------------------
class MeleeAttackBehavior
   : public Behavior
{
public:
   MeleeAttackBehavior(const std::string& name, const XMLNode& blueprintNode);
   MeleeAttackBehavior(const MeleeAttackBehavior& copySource);

   virtual float CalcUtility() override;
   virtual void Run() override;
   virtual Behavior* Clone() override;
   virtual void PopulateFromXMLNode(const XMLNode& blueprintNode);
   virtual bool DoesPassChanceToRun() override;
   virtual void DereferenceEntity(Entity* entity) override;

   static Behavior* CreateBehavior(const std::string& name, const XMLNode& blueprintNode) { return new MeleeAttackBehavior(name, blueprintNode); }
   static BehaviorRegistration s_meleeAttackBehaviorRegistration;

   virtual void WriteToXMLNode(XMLNode&) const override {};

private:
   int m_minDamage;
   int m_maxDamage;
   float m_percentChanceToHit;
   Agent* m_meleeAttackTarget;
};