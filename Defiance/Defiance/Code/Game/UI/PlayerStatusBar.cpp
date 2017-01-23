#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Core/GameContext.hpp"
#include "Game/UI/PlayerStatusBar.hpp"
#include "Game/Entities/Agents/Player.hpp"


//-----------------------------------------------------------------------------------------------
extern PlayerStatusBar* g_thePlayerStatusBar = nullptr;


//-----------------------------------------------------------------------------------------------
PlayerStatusBar::PlayerStatusBar(Player* player)
   : m_color(Rgba(0, 0, 0, 255))
   , m_font(BitmapFont::CreateOrGetFont("CopperplateGothicBold"))
   , m_player(player)
   , m_isGameOver(false)
{}


//-----------------------------------------------------------------------------------------------
void PlayerStatusBar::Render() const
{
   RenderOverlay();

   if (!m_isGameOver)
   {
      RenderStats();
   }
   else
   {
      RenderGameOver();
   }
}


//-----------------------------------------------------------------------------------------------
void PlayerStatusBar::RenderOverlay() const
{
   Vector2f botLeft(10.f, 770.f);
   Vector2f topRight(390.f, 890.f);
   Rgba colorToDraw = m_color;
   if (colorToDraw.a > (.3f * 255))
   {
      colorToDraw.a = (char)(.3f * 255);
   }

   g_theRenderer->DrawAABB2(AABB2(botLeft, topRight), colorToDraw);
}


//-----------------------------------------------------------------------------------------------
void PlayerStatusBar::RenderStats() const
{
   int playerCurrentHealth = m_player->GetCurrentHealth();
   int playerMaxHealth = m_player->GetMaxHealth();
   
   Vector2f healthPosition(20.f, 880.f);
   std::string healthString = Stringf("Health: %d / %d", playerCurrentHealth, playerMaxHealth);
   g_theRenderer->DrawText2D(healthPosition, healthString, Rgba::RED, 24.f, m_font);

   Vector2f turnCountPosition(20.f, 840.f);
   int stepsTaken = g_theGame->GetPlayerStepCount();
   std::string turnCountString = Stringf("Turns Taken: %d", stepsTaken);;
   g_theRenderer->DrawText2D(turnCountPosition, turnCountString, Rgba::WHITE, 24.f, m_font);
}


//-----------------------------------------------------------------------------------------------
void PlayerStatusBar::RenderGameOver() const
{
   GameContext* context = g_theGame->GetGameContext();

   const BitmapFont* font = BitmapFont::CreateOrGetFont("CopperplateGothicBold");
   g_theRenderer->DrawText2D(Vector2f(20.f, 890.f), "Game Over", Rgba::WHITE, 40.f, font);
   g_theRenderer->DrawText2D(Vector2f(20.f, 840.f), Stringf("Turns taken: %d", context->numberOfPlayerStepsTaken), Rgba::WHITE, 20.f, font);
   g_theRenderer->DrawText2D(Vector2f(20.f, 815.f), Stringf("Enemies slain: %d", context->numberOfEnemiesSlainByPlayer), Rgba::WHITE, 20.f, font);
   g_theRenderer->DrawText2D(Vector2f(200.f, 70.f), "Escape - Return To Main Menu", Rgba::WHITE, 20.f, font);
}