#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"


//-----------------------------------------------------------------------------------------------
SpriteAnimation::SpriteAnimation(SpriteSheet* spriteSheet, float durationSeconds, const SpriteAnimMode& mode,
   int startSpriteIndex, int endSpriteIndex)
   : m_spriteSheet(spriteSheet)
   , m_animMode(mode)
   , m_startSpriteIndex(startSpriteIndex)
   , m_endSpriteIndex(endSpriteIndex)
   , m_isFinished(false)
   , m_isPlaying(true)
   , m_durationSeconds(durationSeconds)
   , m_elapsedSeconds(0.f)
   , m_secondsPerFrame(m_durationSeconds / (float)(m_endSpriteIndex - m_startSpriteIndex + 1))
{
}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::Update(float deltaSeconds)
{
   if (!m_isPlaying || m_isFinished)
   {
      return;
   }

   m_elapsedSeconds += deltaSeconds;

   switch (m_animMode)
   {
   case PLAY_TO_END:
   {
      if (m_elapsedSeconds > m_durationSeconds)
      {
         m_isPlaying = false;
         m_isFinished = true;
      }
      
      break;
   }

   /*case PINGPONG: optional
   {
      break;
   }*/

   default: // case LOOPING
   {
      if (m_elapsedSeconds > m_durationSeconds)
      {
         m_elapsedSeconds = 0.f;
      }
   }
   }
}


//-----------------------------------------------------------------------------------------------
TexBounds SpriteAnimation::GetCurrentTexCoords() const
{
   SpriteIndex currentIndex = GetCurrentIndex();
   return m_spriteSheet->GetTexBoundsForSpriteIndex(currentIndex);
}


//-----------------------------------------------------------------------------------------------
SpriteIndex SpriteAnimation::GetCurrentIndex() const
{
   return (SpriteIndex)floor(m_elapsedSeconds / m_secondsPerFrame);
}


//-----------------------------------------------------------------------------------------------
Texture* SpriteAnimation::GetTexture() const
{
   return m_spriteSheet->GetTexture();
}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::Pause()
{
   m_isPlaying = false;
}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::Resume()
{
   m_isPlaying = true;
}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::Reset()
{
   m_elapsedSeconds = 0.f;
   m_isPlaying = true;
   m_isFinished = false;
}


//-----------------------------------------------------------------------------------------------
float SpriteAnimation::GetFractionElapsed() const
{
   return m_elapsedSeconds / m_durationSeconds;
}


//-----------------------------------------------------------------------------------------------
float SpriteAnimation::GetFractionRemaining() const
{
   return 1.f - GetFractionElapsed();
}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::SetSecondsElapsed(float secondsElapsed)
{
   m_elapsedSeconds = secondsElapsed;
}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::SetFractionElapsed(float fractionElapsed)
{
   m_elapsedSeconds = (fractionElapsed * m_durationSeconds);
}