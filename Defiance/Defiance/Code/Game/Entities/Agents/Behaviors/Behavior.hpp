#pragma once

#include <string>
#include <map>


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class Agent;
class Entity;


//----------------------------------------------------------------------------------------------_
class Behavior
{
public:
   Behavior(const std::string& name, const XMLNode& behaviorNode);
   Behavior(const Behavior& copySource);
   virtual ~Behavior() {}

   virtual float CalcUtility() = 0;
   virtual void Run() = 0;
   virtual Behavior* Clone() = 0;
   virtual void PopulateFromXMLNode(const XMLNode& blueprintNode) = 0;
   virtual bool DoesPassChanceToRun() { return false; }
   virtual void DereferenceEntity(Entity*) {}

   const std::string& GetName() const { return m_name; }
   void SetOwningAgent(Agent* agent) { m_owningAgent = agent; }

   // coincidentally, I never included functionality that required this...
   virtual void WriteToXMLNode(XMLNode& topNode) const;

protected:
   Agent* m_owningAgent;
   std::string m_name;
   float m_chanceToRun;
};


//-----------------------------------------------------------------------------------------------
typedef Behavior* (BehaviorCreationFunc)(const std::string& name, const XMLNode& behaviorNode);


//-----------------------------------------------------------------------------------------------
class BehaviorRegistration;
typedef std::map<std::string, BehaviorRegistration*> BehaviorRegistrationMap;


//-----------------------------------------------------------------------------------------------
class BehaviorRegistration
{
public:
   BehaviorRegistration(const std::string& name, BehaviorCreationFunc* creationFunc);
   BehaviorRegistration() {}

   static Behavior* CreateBehaviorByName(const std::string& name, const XMLNode& behaviorNode);
   static void FreeAllBehaviorRegistrations();
   static BehaviorRegistrationMap* s_behaviorRegistrationMap;

private:
   std::string m_name;
   BehaviorCreationFunc* m_creationFunc;
};
typedef std::pair<std::string, BehaviorRegistration*> BehaviorRegistrationPair;