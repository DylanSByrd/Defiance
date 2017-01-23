#pragma once

#include <string>
#include "Game/Entities/Agents/Behaviors/Behavior.hpp"
#include "Game/Core/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class Entity;


//-----------------------------------------------------------------------------------------------
class PickUpItemBehavior
   : public Behavior
{
public:
   PickUpItemBehavior(const std::string& name, const XMLNode& blueprintNode);
   PickUpItemBehavior(const PickUpItemBehavior& copySource);

   virtual float CalcUtility() override;
   virtual void Run() override;
   virtual Behavior* Clone() override;
   virtual void PopulateFromXMLNode(const XMLNode& blueprintNode);
   virtual bool DoesPassChanceToRun() override;
   virtual void DereferenceEntity(Entity* entity) override;

   static Behavior* CreateBehavior(const std::string& name, const XMLNode& blueprintNode) { return new PickUpItemBehavior(name, blueprintNode); }
   static BehaviorRegistration s_pickUpItemBehaviorRegistration;

private:
   int m_tilesToTravelForItem;
   Item* m_targetItem;
};