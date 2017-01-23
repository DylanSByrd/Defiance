#include <string>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Entity.hpp"
#include "Game/Map/Map.hpp"


//-----------------------------------------------------------------------------------------------
STATIC int Entity::s_nextID = 1;
STATIC const int Entity::INVALID_ENTITY_ID = -1;


//-----------------------------------------------------------------------------------------------
Entity::Entity()
   : m_currentHealth(1)
   , m_maxHealth(1)
   , m_ID(s_nextID)
   , m_entityType(INVALID_ENTITY_TYPE)
   , m_position(TileCoords::ZERO)
   , m_color(Rgba::YELLOW)
   , m_backgroundColor(Rgba::BLACK)
   , m_name("Invalid Entity")
{
}


//-----------------------------------------------------------------------------------------------
Entity::Entity(const XMLNode& blueprintNode)
   : Entity()
{
   if (!strcmp(blueprintNode.getName(), "NPCBlueprint")
      || !strcmp(blueprintNode.getName(), "PlayerBlueprint"))
   {
      m_entityType = AGENT_TYPE;
   }

   if (!strcmp(blueprintNode.getName(), "ItemBlueprint"))
   {
      m_entityType = ITEM_TYPE;
   }

   if (!strcmp(blueprintNode.getName(), "FeatureBlueprint"))
   {
      m_entityType = FEATURE_TYPE;
   }

   m_color = ReadXMLAttribute(blueprintNode, "color", m_color);
   m_backgroundColor = ReadXMLAttribute(blueprintNode, "backgroundColor", m_backgroundColor);
   m_name = ReadXMLAttribute(blueprintNode, "name", std::string(""));
}


//-----------------------------------------------------------------------------------------------
Entity::Entity(int health, EntityType type, const TileCoords& position, const Rgba& color, const Rgba& backgroundColor, const std::string& name)
   : m_currentHealth(health)
   , m_maxHealth(health)
   , m_ID(s_nextID++)
   , m_entityType(type)
   , m_position(position)
   , m_color(color)
   , m_backgroundColor(backgroundColor)
   , m_name(name)
{
}


//-----------------------------------------------------------------------------------------------
Entity::Entity(const Entity& copySource)
   : m_currentHealth(copySource.m_currentHealth)
   , m_maxHealth(copySource.m_maxHealth)
   , m_ID(s_nextID++)
   , m_entityType(copySource.m_entityType)
   , m_position(copySource.m_position)
   , m_color(copySource.m_color)
   , m_backgroundColor(copySource.m_backgroundColor)
   , m_name(copySource.m_name)
{
}


//-----------------------------------------------------------------------------------------------
void Entity::AddToMap(Map* map, const TileCoords& position)
{
   m_gameMap = map;
   m_position = position;
}


// #TODO - move some of this farther up so items don't have like 1 health.
//-----------------------------------------------------------------------------------------------
bool Entity::WriteToXMLNode(XMLNode& entityNode) const
{
   bool writeSuccessful = true;

   // Used for itoa and the like
   char spareBuf[255];

   entityNode.addAttribute("name", m_name.c_str());

   _itoa_s(m_ID, spareBuf, 10);
   entityNode.addAttribute("EntityID", spareBuf);

   _itoa_s(m_maxHealth, spareBuf, 10);
   entityNode.addAttribute("maxHealth", spareBuf);

   if (m_currentHealth != m_maxHealth)
   {
      _itoa_s(m_currentHealth, spareBuf, 10);
      entityNode.addAttribute("currentHealth", spareBuf);
   }

   entityNode.addAttribute("position", m_position.ToString().c_str());

   return writeSuccessful;
}


//-----------------------------------------------------------------------------------------------
void Entity::LoadFromXMLNode(const XMLNode& saveNode)
{
   // #TODO - move to entity
   m_maxHealth = ReadXMLAttribute(saveNode, "maxHealth", m_maxHealth);
   m_currentHealth = ReadXMLAttribute(saveNode, "currentHealth", m_maxHealth);
   m_position = ReadXMLAttribute(saveNode, "position", m_position);
}


//-----------------------------------------------------------------------------------------------
bool Entity::operator==(const Entity& rhs) const
{
   bool entitiesAreEqual = true;

   if (m_ID == rhs.m_ID)
   {
      return entitiesAreEqual;
   }

   return !entitiesAreEqual;
}
