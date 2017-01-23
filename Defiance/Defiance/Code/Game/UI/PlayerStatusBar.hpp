#pragma once


//-----------------------------------------------------------------------------------------------
class PlayerStatusBar;
extern PlayerStatusBar* g_thePlayerStatusBar;


//-----------------------------------------------------------------------------------------------
class Player;


//-----------------------------------------------------------------------------------------------
class PlayerStatusBar
{
public:
   PlayerStatusBar(Player* player);
   ~PlayerStatusBar() {}

   void Render() const;
   void RenderOverlay() const;
   void RenderStats() const;
   void RenderGameOver() const;

   void SetGameOver() { m_isGameOver = true; }

private:
   Rgba m_color;
   BitmapFont* m_font;
   Player* m_player;
   bool m_isGameOver;
};