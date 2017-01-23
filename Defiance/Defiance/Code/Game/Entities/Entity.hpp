#pragma once

#include <string>
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Math/Vector2i.hpp"
#include "Game/Core/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
class Map;
struct XMLNode;


//-----------------------------------------------------------------------------------------------
enum EntityType
{
   INVALID_ENTITY_TYPE = -1,
   ITEM_TYPE,
   AGENT_TYPE,
   FEATURE_TYPE,
   NUM_ENTITY_TYPES,
};


//-----------------------------------------------------------------------------------------------
class Entity
{
public:
   Entity();
   Entity(const XMLNode& blueprintNode);
   Entity(int health, EntityType type, const TileCoords& position, const Rgba& color, const Rgba& backgroundColor, const std::string& name);
   Entity(const Entity& copySource);
   virtual ~Entity() {}

   virtual bool IsReadyToUpdate() const { return true; }
   virtual float Update() { return 1.f; };
   virtual bool IsPlayer() const { return false; }
   virtual bool IsAgent() const { return false; }
   virtual bool IsItem() const { return false; }
   virtual bool IsFeature() const { return false; }

   virtual void AddToMap(Map* map, const TileCoords& position);
   virtual void DereferenceEntity(Entity*) {}

   EntityType GetEntityType() const { return m_entityType; }
   EntityID GetID() const { return m_ID; }
   Map* GetMap() const { return m_gameMap; }
   int GetCurrentHealth() const { return m_currentHealth; }
   int GetMaxHealth() const { return m_maxHealth; }
   const Rgba& GetColor() const { return m_color; }
   const TileCoords& GetPosition() const { return m_position; }
   const std::string& GetName() const { return m_name; }
   bool IsAlive() const { return m_currentHealth > 0; }
   bool IsAtMaxHealth() const { return m_currentHealth == m_maxHealth; }

   void SetMaxHealth(int newMaxHealth) { m_maxHealth = newMaxHealth; }
   void SetCurrentHealth(int newCurrentHealth) { m_currentHealth = newCurrentHealth; }
   void SetPosition(const TileCoords& position) { m_position = position; }

   virtual bool WriteToXMLNode(XMLNode& entityNode) const;
   virtual void LoadFromXMLNode(const XMLNode& saveNode);

   virtual void ResolveEntityPointers(const std::map<int, Entity*>&) {}


   bool virtual operator==(const Entity& rhs) const;

   static int s_nextID;
   static const int INVALID_ENTITY_ID;

protected:
   int m_currentHealth;
   int m_maxHealth;
   EntityID m_ID;
   EntityType m_entityType;
   TileCoords m_position;
   Rgba m_color;
   Rgba m_backgroundColor;
   std::string m_name;
   Map* m_gameMap;
};