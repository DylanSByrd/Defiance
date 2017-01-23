#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Agents/Player.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/FieldOfView/FieldOfViewAdvanced.hpp"
#include "Game/Combat/DefianceCombatSystem.hpp"
#include "Game/UI/GameMessageBox.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player(int health, EntityType type, const TileCoords& position, char glyph, const Rgba& color, const Rgba& backgroundColor, const std::string& name)
   : Agent(health, type, position, glyph, color, backgroundColor, name)
   , m_nextIntendedAction(PLAYER_ACTION_UNSPECIFIED)
   , m_nextIntendedMoveDirection(NORTH)
   , m_isReadyToUpdate(false)
   , m_speedMultiplier(1.f)
   , m_turnsUntilSpeedBuffFinished(0)
{
   m_faction = *Faction::CreateOrGetFactionByName("Player");
}


//-----------------------------------------------------------------------------------------------
Player::Player(const XMLNode& playerNode)
   :Agent(0, AGENT_TYPE, Vector2i::ZERO, '@', Rgba::YELLOW, Rgba::BLACK, "Player")
   , m_nextIntendedAction(PLAYER_ACTION_UNSPECIFIED)
   , m_nextIntendedMoveDirection(NORTH)
   , m_isReadyToUpdate(false)
   , m_speedMultiplier(1.f)
   , m_turnsUntilSpeedBuffFinished(0)
{
   m_faction = *Faction::CreateOrGetFactionByName("Player"); 
   PopulateFromXMLNode(playerNode);
}


//-----------------------------------------------------------------------------------------------
float Player::Update()
{
   float timeToUpdate = 0.f;

   switch (m_nextIntendedAction)
   {
   case PLAYER_ACTION_MOVE:
   {
      const Tile& tile = m_gameMap->GetTileInDirection(m_position, m_nextIntendedMoveDirection);

      if (tile.IsOccupiedByAgent())
      {
         AttackAdjacentAgent(tile.occupyingAgent);
      }
      else
      {
         MoveOneStepInDirection(m_nextIntendedMoveDirection);
      }
      timeToUpdate += 1.f;
      break;
   }

   case PLAYER_ACTION_GET_ITEM:
   {
      bool pickedUpItem = TryGetItemFromCurrentTile();
      if (!pickedUpItem)
      {
         g_theGameMessageBox->PrintCautionMessage("You look for something to pick up and find nothing.");
      }
      else
      {
         timeToUpdate += 1.f;
      }
      break;
   }

   case PLAYER_ACTION_USE_ITEM:
   {
      if (m_inventory.GetNumItemsOfTypeInInventory(CONSUMABLE_ITEM) == 0)
      {
         g_theGameMessageBox->PrintDangerMessage("You have no items to use!");
      }
      else
      {
         UseRandomItem();
         timeToUpdate += 1.f;
      }
      break;
   }

   case PLAYER_ACTION_TOGGLE_FEATURE:
   {
      bool didToggleFeatures = ToggleNearbyFeatures();
      if (!didToggleFeatures)
      {
         g_theGameMessageBox->PrintCautionMessage("You look for a door to move but find none.");
      }
      else
      {
         timeToUpdate += .2f;
      }
      break;
   }

   case PLAYER_ACTION_WAIT:
   {
      timeToUpdate += 1.f;
   }
   }

   // If failed to do anything, no turn passed
   if (timeToUpdate > 0.f)
   {
      // This would be better to access in a global game context instead of the game itself
      g_theGame->IncrementPlayerTurnCount();

      if (m_turnsUntilSpeedBuffFinished > 0)
      {
         --m_turnsUntilSpeedBuffFinished;

         if (m_turnsUntilSpeedBuffFinished == 0)
         {
            m_speedMultiplier = 1.f;
            g_theGameMessageBox->PrintCautionMessage("You feel the adrenaline wear off!");
         }
      }
   }

   UpdateFOV();
   m_nextIntendedAction = PLAYER_ACTION_UNSPECIFIED;
   return timeToUpdate / m_speedMultiplier;
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFOV()
{
   m_visibleAgents.clear();
   FieldOfViewAdvanced fov;
   fov.CalculateFieldOfViewForAgent(this, 20, m_gameMap, true);
}


//-----------------------------------------------------------------------------------------------
void Player::HandleInput()
{
   if (g_theInputSystem->WasKeyJustReleased('H')
      || g_theInputSystem->WasKeyJustReleased(VK_NUMPAD4))
   {
      m_nextIntendedAction = PLAYER_ACTION_MOVE;
      m_nextIntendedMoveDirection = WEST;
   }

   if (g_theInputSystem->WasKeyJustReleased('J')
      || g_theInputSystem->WasKeyJustReleased(VK_NUMPAD2))
   {
      m_nextIntendedAction = PLAYER_ACTION_MOVE;
      m_nextIntendedMoveDirection = SOUTH;
   }

   if (g_theInputSystem->WasKeyJustReleased('K')
      || g_theInputSystem->WasKeyJustReleased(VK_NUMPAD8))
   {
      m_nextIntendedAction = PLAYER_ACTION_MOVE;
      m_nextIntendedMoveDirection = NORTH;
   }

   if (g_theInputSystem->WasKeyJustReleased('L')
      || g_theInputSystem->WasKeyJustReleased(VK_NUMPAD6))
   {
      m_nextIntendedAction = PLAYER_ACTION_MOVE;
      m_nextIntendedMoveDirection = EAST;
   }

   if (g_theInputSystem->WasKeyJustReleased('Y')
      || g_theInputSystem->WasKeyJustReleased(VK_NUMPAD7))
   {
      m_nextIntendedAction = PLAYER_ACTION_MOVE;
      m_nextIntendedMoveDirection = NORTH_WEST;
   }

   if (g_theInputSystem->WasKeyJustReleased('U')
      || g_theInputSystem->WasKeyJustReleased(VK_NUMPAD9))
   {
      m_nextIntendedAction = PLAYER_ACTION_MOVE;
      m_nextIntendedMoveDirection = NORTH_EAST;
   }

   if (g_theInputSystem->WasKeyJustReleased('B')
      || g_theInputSystem->WasKeyJustReleased(VK_NUMPAD1))
   {
      m_nextIntendedAction = PLAYER_ACTION_MOVE;
      m_nextIntendedMoveDirection = SOUTH_WEST;
   }

   if (g_theInputSystem->WasKeyJustReleased('N')
      || g_theInputSystem->WasKeyJustReleased(VK_NUMPAD3))
   {
      m_nextIntendedAction = PLAYER_ACTION_MOVE;
      m_nextIntendedMoveDirection = SOUTH_EAST;
   }

   if (g_theInputSystem->WasKeyJustReleased(VK_SPACE)
      || g_theInputSystem->WasKeyJustReleased(VK_NUMPAD5))
   {
      m_nextIntendedAction = PLAYER_ACTION_WAIT;
   }

   if (g_theInputSystem->WasKeyJustReleased(188)) // ','
   {
      m_nextIntendedAction = PLAYER_ACTION_GET_ITEM;
   }

   if (g_theInputSystem->WasKeyJustReleased('Q'))
   {
      m_nextIntendedAction = PLAYER_ACTION_USE_ITEM;
   }

   if (g_theInputSystem->WasKeyJustReleased('A'))
   {
      m_nextIntendedAction = PLAYER_ACTION_TOGGLE_FEATURE;
   }

   if ((g_theInputSystem->IsKeyDown('S') && g_theInputSystem->IsKeyDown(VK_SHIFT))
      || g_theInputSystem->WasKeyJustPressed(VK_F6))
   {
      g_theGame->SaveGameToFile();
   }
}


// Could probably utilize behaviors here instead, but hard coding for now
//-----------------------------------------------------------------------------------------------
void Player::AttackAdjacentAgent(Agent* target)
{
   AttackData attackData;
   attackData.minDamage = 2;
   attackData.maxDamage = 4;
   attackData.percentChanceToHit = 90.f;
   attackData.instigator = this;
   attackData.target = target;

   PerformAttack(attackData);
}


//-----------------------------------------------------------------------------------------------
void Player::UseRandomItem()
{
   Item* itemToUse = nullptr;
   m_inventory.GetItemOfType(CONSUMABLE_ITEM, &itemToUse);

   // #TODO - remove item from game
   if (itemToUse->IsSpeedBuff())
   {
      m_speedMultiplier = 2.f;
      m_turnsUntilSpeedBuffFinished = 8;
      
      g_theGameMessageBox->PrintGoodMessage("You use the shot of adrenaline and feel the world move slower!");

      itemToUse->SetCurrentHealth(0);
      return;
   }

   // Is medkit
   const IntRange& healingPower = itemToUse->GetHealingPower();
   int healAmount = GetRandomIntBetweenInclusive(healingPower);

   m_currentHealth += healAmount;
   if (m_currentHealth > m_maxHealth)
   {
      m_currentHealth = m_maxHealth;
   }

   itemToUse->SetCurrentHealth(0);
   g_theGameMessageBox->PrintGoodMessage(Stringf("You use the medkit to heal yourself for %d health!", healAmount));
}


//-----------------------------------------------------------------------------------------------
bool Player::ToggleNearbyFeatures()
{
   bool foundFeatureToToggle = false;

   for (size_t directionIndex = 0; directionIndex < NUM_CARDINAL_DIRECTIONS; ++directionIndex)
   {
      TileCoords coordsToCheck = m_gameMap->GetTileCoordsInDirection(m_position, (TileDirection)directionIndex);
      Tile* tileToToggle = m_gameMap->GetTileAtTileCoords(coordsToCheck);

      if (tileToToggle->HasAFeature())
      {
         tileToToggle->ToggleFeature();
         foundFeatureToToggle = true;
      }
   }

   return foundFeatureToToggle;
}


//-----------------------------------------------------------------------------------------------
bool Player::WriteToXMLNode(XMLNode& entityNode) const
{
   bool writeSuccessful = true;

   XMLNode playerNode = entityNode.addChild("PlayerBlueprint");
   if (!Agent::WriteToXMLNode(playerNode))
   {
      return !writeSuccessful;
   }

   // Used for itoa and the like
   char spareBuf[255];

   if (m_turnsUntilSpeedBuffFinished > 0)
   {
      _itoa_s(m_turnsUntilSpeedBuffFinished, spareBuf, 10);
      entityNode.addAttribute("turnsRemainingForSpeedBuff", spareBuf);
   }

   return writeSuccessful;
}


//-----------------------------------------------------------------------------------------------
bool Player::IsReadyToUpdate() const
{
   bool readyToUpdate = true;
   if (m_nextIntendedAction != PLAYER_ACTION_UNSPECIFIED)
   {
      return readyToUpdate;
   }

   return !readyToUpdate;
}


//-----------------------------------------------------------------------------------------------
void Player::PopulateFromXMLNode(const XMLNode& playerNode)
{
   Agent::LoadFromXMLNode(playerNode);
   
   m_turnsUntilSpeedBuffFinished = ReadXMLAttribute(playerNode, "turnsRemainingForSpeedBuff", 0);
   if (m_turnsUntilSpeedBuffFinished != 0)
   {
      m_speedMultiplier = 2.f;
   }
}