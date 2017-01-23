#pragma once

#include <string>
#include "Game/Entities/Agents/Behaviors/Behavior.hpp"
#include "Game/Core/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class Entity;


//-----------------------------------------------------------------------------------------------
class FleeBehavior
   : public Behavior
{
public:
   FleeBehavior(const std::string& name, const XMLNode& blueprintNode);
   FleeBehavior(const FleeBehavior& copySource);

   virtual float CalcUtility() override;
   virtual void Run() override;
   virtual Behavior* Clone() override;
   virtual void PopulateFromXMLNode(const XMLNode& blueprintNode);
   virtual bool DoesPassChanceToRun() override;

   static Behavior* CreateBehavior(const std::string& name, const XMLNode& blueprintNode) { return new FleeBehavior(name, blueprintNode); }
   static BehaviorRegistration s_fleeBehaviorRegistration;

   virtual void WriteToXMLNode(XMLNode&) const override {};

private: // #TODO make threshold a percentage of max health
   int m_healthThreshold;
   bool m_isFleeing;
};