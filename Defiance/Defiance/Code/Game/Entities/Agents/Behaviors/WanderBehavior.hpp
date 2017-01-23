#pragma once

#include <string>
#include "Game/Entities/Agents/Behaviors/Behavior.hpp"
#include "Game/Core/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
class WanderBehavior
   : public Behavior
{
public:
   WanderBehavior(const std::string& name, const XMLNode& blueprintNode);
   WanderBehavior(const WanderBehavior& copySource);

   virtual float CalcUtility() override;
   virtual void Run() override;
   virtual Behavior* Clone() override;
   virtual void PopulateFromXMLNode(const XMLNode& blueprintNode) override;
   virtual bool DoesPassChanceToRun() override;

   static Behavior* CreateBehavior(const std::string& name, const XMLNode& blueprintNode) { return new WanderBehavior(name, blueprintNode); }
   static BehaviorRegistration s_wanderBehaviorRegistrastion;

   virtual void WriteToXMLNode(XMLNode&) const override {};

private:
   float m_chanceToRest;
   float m_chanceToGoStraight;
   TileDirection m_lastDirection;
};