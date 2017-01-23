#pragma once

#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
class SpriteSheet;
class AABB2;
class Texture;

//-----------------------------------------------------------------------------------------------
class SpriteAnimation
{
public:
   enum SpriteAnimMode
   {
      PLAY_TO_END,
      //PINGPONG, optional; might get to
      LOOPING,
      NUM_SPRITE_ANIM_MODES
   };

   SpriteAnimation(SpriteSheet* spriteSheet, float durationSeconds, const SpriteAnimMode& mode,
      int startSpriteIndex, int endSpriteIndex);

   void Update(float deltaSeconds);
   TexBounds GetCurrentTexCoords() const;
   SpriteIndex GetCurrentIndex() const;
   Texture* GetTexture() const;
   void Pause();
   void Resume();
   void Reset();
   float GetSecondsPerFrame() const { return m_secondsPerFrame; }
   bool IsFinished() const { return m_isFinished; }
   bool IsPlaying() const { return m_isPlaying; }
   float GetDurationSeconds() const { return m_durationSeconds; }
   float GetSecondsElapsed() const { return m_elapsedSeconds; }
   float GetFractionElapsed() const;
   float GetFractionRemaining() const;
   void SetSecondsElapsed(float secondsElapsed);
   void SetFractionElapsed(float fractionElapsed);

private:
   SpriteSheet* m_spriteSheet;
   SpriteAnimMode m_animMode;
   int m_startSpriteIndex;
   int m_endSpriteIndex;
   bool m_isFinished;
   bool m_isPlaying;
   float m_durationSeconds;
   float m_elapsedSeconds;
   float m_secondsPerFrame;
};