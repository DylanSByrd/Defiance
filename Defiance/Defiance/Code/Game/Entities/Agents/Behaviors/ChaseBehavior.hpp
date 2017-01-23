#pragma once

#include <string>
#include "Game/Entities/Agents/Behaviors/Behavior.hpp"
#include "Game/Core/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class Entity;


//-----------------------------------------------------------------------------------------------
class ChaseBehavior
   : public Behavior 
{
public:
   ChaseBehavior(const std::string& name, const XMLNode& blueprintNode);
   ChaseBehavior(const ChaseBehavior& copySource);

   virtual float CalcUtility() override;
   virtual void Run() override;
   virtual Behavior* Clone() override;
   virtual void PopulateFromXMLNode(const XMLNode& blueprintNode);
   virtual bool DoesPassChanceToRun() override;
   virtual void DereferenceEntity(Entity* entity) override;

   static Behavior* CreateBehavior(const std::string& name, const XMLNode& blueprintNode) { return new ChaseBehavior(name, blueprintNode); }
   static BehaviorRegistration s_chaseBehaviorRegistration;

   virtual void WriteToXMLNode(XMLNode&) const override {};

private:
   int m_tilesFromStartToChase;
   int m_turnsToChase;
   Agent* m_chaseTarget;
};