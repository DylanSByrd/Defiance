#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Agents/Behaviors/Behavior.hpp"


//-----------------------------------------------------------------------------------------------
STATIC BehaviorRegistrationMap* BehaviorRegistration::s_behaviorRegistrationMap = nullptr;


//-----------------------------------------------------------------------------------------------
Behavior::Behavior(const std::string& name, const XMLNode& blueprintNode)
   : m_name(name)
   , m_chanceToRun(1.f)
{
   m_chanceToRun = ReadXMLAttribute(blueprintNode, "chanceToRun", m_chanceToRun);
}


//-----------------------------------------------------------------------------------------------
Behavior::Behavior(const Behavior& copySource)
   : m_name(copySource.m_name)
   , m_chanceToRun(copySource.m_chanceToRun)
{
}


//-----------------------------------------------------------------------------------------------
void Behavior::WriteToXMLNode(XMLNode& topNode) const
{
   XMLNode behaviorNode = topNode.addChild(m_name.c_str());
}


//-----------------------------------------------------------------------------------------------
BehaviorRegistration::BehaviorRegistration(const std::string& name, BehaviorCreationFunc* creationFunc)
   : m_name(name)
   , m_creationFunc(creationFunc)
{
   ASSERT_OR_DIE(creationFunc != nullptr, Stringf("Error: no creation function provided for behavior %s!", m_name.c_str()));

   if (!s_behaviorRegistrationMap)
   {
      s_behaviorRegistrationMap = new BehaviorRegistrationMap;
   }

   ASSERT_OR_DIE(s_behaviorRegistrationMap->find(m_name) == s_behaviorRegistrationMap->end(), Stringf("Error: behavior %s already registered!", m_name.c_str()));
   s_behaviorRegistrationMap->insert(BehaviorRegistrationPair(m_name, this));
}


//-----------------------------------------------------------------------------------------------
STATIC Behavior* BehaviorRegistration::CreateBehaviorByName(const std::string& name, const XMLNode& behaviorNode)
{
   BehaviorRegistration* behaviorRegistration = nullptr;
   ASSERT_OR_DIE(s_behaviorRegistrationMap != nullptr, "Error: Behavior registration map failed to initialize!");

   BehaviorRegistrationMap::iterator registrationIter = s_behaviorRegistrationMap->find(name);
   ASSERT_OR_DIE(registrationIter != s_behaviorRegistrationMap->end(), Stringf("Error: Behavior %s not registered!", name.c_str()));

   behaviorRegistration = registrationIter->second;
   Behavior* behavior = nullptr;
   behavior = (*behaviorRegistration->m_creationFunc)(name, behaviorNode);
   return behavior;
}


//-----------------------------------------------------------------------------------------------
STATIC void BehaviorRegistration::FreeAllBehaviorRegistrations()
{
   delete s_behaviorRegistrationMap;
}