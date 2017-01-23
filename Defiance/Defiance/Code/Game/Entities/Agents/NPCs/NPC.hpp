#pragma once

#include "Game/Entities/Agents/Agent.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
class NPC
   : public Agent
{
public:
   NPC(const XMLNode& blueprintNode);
   NPC(const NPC& copySource);
   NPC(const NPC& copySource, const XMLNode& node);

   void PopulateFromXMLNode(const XMLNode& blueprintNode);
   void PopulateBehaviorsFromXMLNode(const XMLNode& behaviorRootNode);

   virtual bool WriteToXMLNode(XMLNode& entityNode) const override;
};