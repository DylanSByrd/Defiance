#pragma once

#include "Game/Entities/Agents/Agent.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
enum PlayerAction
{
   PLAYER_ACTION_UNSPECIFIED,
   PLAYER_ACTION_MOVE,
   PLAYER_ACTION_GET_ITEM,
   PLAYER_ACTION_USE_ITEM,
   PLAYER_ACTION_TOGGLE_FEATURE,
   PLAYER_ACTION_WAIT,
   NUM_PLAYER_ACTIONS,
};


//-----------------------------------------------------------------------------------------------
class Player
   : public Agent
{
public:
   Player(int health, EntityType type, const TileCoords& position, char glyph, const Rgba& color, const Rgba& backgroundColor, const std::string& name);
   Player(const XMLNode& playerNode);

   virtual float Update() override;
   virtual void UpdateFOV() override;
   void HandleInput();

   void AttackAdjacentAgent(Agent* target);
   void UseRandomItem();
   bool ToggleNearbyFeatures();

   virtual bool WriteToXMLNode(XMLNode& entityNode) const override;

   bool IsPlayer() const { return true; }
   virtual bool IsReadyToUpdate() const override;

private:
   void PopulateFromXMLNode(const XMLNode& playerNode);

   PlayerAction m_nextIntendedAction;
   TileDirection m_nextIntendedMoveDirection;
   bool m_isReadyToUpdate;
   float m_speedMultiplier;
   int m_turnsUntilSpeedBuffFinished;
};