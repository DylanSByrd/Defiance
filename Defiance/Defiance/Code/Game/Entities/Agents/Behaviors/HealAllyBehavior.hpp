#pragma once

#include <string>
#include "Game/Entities/Agents/Behaviors/Behavior.hpp"
#include "Game/Core/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class Entity;


//-----------------------------------------------------------------------------------------------
class HealAllyBehavior
   : public Behavior
{
public:
   HealAllyBehavior(const std::string& name, const XMLNode& blueprintNode);
   HealAllyBehavior(const HealAllyBehavior& copySource);

   virtual float CalcUtility() override;
   virtual void Run() override;
   virtual Behavior* Clone() override;
   virtual void PopulateFromXMLNode(const XMLNode& blueprintNode);
   virtual bool DoesPassChanceToRun() override;
   virtual void DereferenceEntity(Entity* entity) override;

   static Behavior* CreateBehavior(const std::string& name, const XMLNode& blueprintNode) { return new HealAllyBehavior(name, blueprintNode); }
   static BehaviorRegistration s_healAllyBehaviorRegistration;

   virtual void WriteToXMLNode(XMLNode&) const override {};

private:
   int m_minHealthToHeal;
   int m_maxHealthToHeal;
   float m_percentChanceToHit;
   Agent* m_healTarget;
};